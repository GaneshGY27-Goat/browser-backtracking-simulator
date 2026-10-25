#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include "browser.h"

// Cross-platform socket headers
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_t;
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

// Global Browser instance managing the LIFO stack
static Browser g_browser;

// Helper: Escape JSON strings
std::string escapeJson(const std::string& input) {
    std::ostringstream ss;
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        if (c == '"') ss << "\\\"";
        else if (c == '\\') ss << "\\\\";
        else if (c == '\b') ss << "\\b";
        else if (c == '\f') ss << "\\f";
        else if (c == '\n') ss << "\\n";
        else if (c == '\r') ss << "\\r";
        else if (c == '\t') ss << "\\t";
        else ss << c;
    }
    return ss.str();
}

// Helper: Flexible JSON string field extraction
// Handles {"url": "..."} or {url: "..."} or single quotes
std::string extractJsonField(const std::string& json, const std::string& key) {
    std::string searchWithQuotes = "\"" + key + "\"";
    size_t keyPos = json.find(searchWithQuotes);
    if (keyPos == std::string::npos) {
        keyPos = json.find(key);
        if (keyPos == std::string::npos) return "";
    }

    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) return "";

    size_t quoteStart = json.find_first_of("\"'", colonPos + 1);
    if (quoteStart == std::string::npos) return "";

    char quoteChar = json[quoteStart];
    size_t quoteEnd = json.find(quoteChar, quoteStart + 1);
    if (quoteEnd == std::string::npos) return "";

    return json.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
}

// Helper: Extract query parameter from path (e.g. /api/visit?url=...)
std::string extractQueryParam(const std::string& path, const std::string& param) {
    std::string search = param + "=";
    size_t pos = path.find(search);
    if (pos == std::string::npos) return "";

    size_t start = pos + search.length();
    size_t end = path.find('&', start);
    if (end == std::string::npos) {
        return path.substr(start);
    }
    return path.substr(start, end - start);
}

// Helper: Build the JSON state string
std::string buildStateJson(bool success = true, const std::string& message = "") {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"success\": " << (success ? "true" : "false") << ",\n";
    if (!message.empty()) {
        ss << "  \"message\": \"" << escapeJson(message) << "\",\n";
    }
    ss << "  \"currentPage\": \"" << escapeJson(g_browser.showCurrentPage()) << "\",\n";
    ss << "  \"historySize\": " << g_browser.getHistorySize() << ",\n";
    ss << "  \"canGoBack\": " << (g_browser.isHistoryEmpty() ? "false" : "true") << ",\n";
    ss << "  \"history\": [\n";

    std::vector<std::string> history = g_browser.showHistory();
    for (size_t i = 0; i < history.size(); ++i) {
        ss << "    \"" << escapeJson(history[i]) << "\"";
        if (i + 1 < history.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ]\n";
    ss << "}";
    return ss.str();
}

// Predefined websites JSON response
std::string getWebsitesJson() {
    return "{\n"
           "  \"websites\": [\n"
           "    {\"id\": \"youtube\", \"name\": \"YouTube\", \"url\": \"https://www.youtube.com/\"},\n"
           "    {\"id\": \"chrome\", \"name\": \"Google Chrome\", \"url\": \"https://www.google.com/intl/en_in/chrome/\"},\n"
           "    {\"id\": \"instagram\", \"name\": \"Instagram\", \"url\": \"https://www.instagram.com/?hl=en\"},\n"
           "    {\"id\": \"rscoe\", \"name\": \"Learner RSCOE Campus\", \"url\": \"https://learnerrscoe.edupluscampus.com/\"}\n"
           "  ]\n"
           "}";
}

// Read static file from public directory (or current directory fallback)
bool readStaticFile(const std::string& path, std::string& content, std::string& contentType) {
    std::vector<std::string> potentialPaths;
    potentialPaths.push_back("public" + path);
    potentialPaths.push_back("." + path);
    if (path == "/") {
        potentialPaths.push_back("public/index.html");
        potentialPaths.push_back("index.html");
    }

    if (path.find(".html") != std::string::npos || path == "/") {
        contentType = "text/html; charset=utf-8";
    } else if (path.find(".css") != std::string::npos) {
        contentType = "text/css; charset=utf-8";
    } else if (path.find(".js") != std::string::npos) {
        contentType = "application/javascript; charset=utf-8";
    } else if (path.find(".json") != std::string::npos) {
        contentType = "application/json; charset=utf-8";
    } else if (path.find(".svg") != std::string::npos) {
        contentType = "image/svg+xml";
    } else if (path.find(".ico") != std::string::npos) {
        contentType = "image/x-icon";
    } else {
        contentType = "text/plain; charset=utf-8";
    }

    for (size_t i = 0; i < potentialPaths.size(); ++i) {
        std::ifstream file(potentialPaths[i].c_str(), std::ios::in | std::ios::binary);
        if (file.is_open()) {
            std::ostringstream ss;
            ss << file.rdbuf();
            content = ss.str();
            return true;
        }
    }
    return false;
}

// Send HTTP response helper
void sendHttpResponse(SOCKET clientSocket, int statusCode, const std::string& statusText,
                      const std::string& contentType, const std::string& body, bool sendBody = true) {
    std::ostringstream response;
    response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    response << "Content-Type: " << contentType << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Access-Control-Allow-Origin: *\r\n";
    response << "Access-Control-Allow-Methods: GET, POST, OPTIONS, HEAD\r\n";
    response << "Access-Control-Allow-Headers: Content-Type\r\n";
    response << "Connection: close\r\n\r\n";

    if (sendBody) {
        response << body;
    }

    std::string respStr = response.str();
    send(clientSocket, respStr.c_str(), (int)respStr.length(), 0);
}

int main() {
    int port = 8080;
    const char* portEnv = std::getenv("PORT");
    if (portEnv != NULL) {
        port = std::atoi(portEnv);
    }

    std::cout << "==================================================" << std::endl;
    std::cout << "  Browser Backtracking Simulator - C++ Backend   " << std::endl;
    std::cout << "  Stack STL Navigation Engine (LIFO Demo)        " << std::endl;
    std::cout << "==================================================" << std::endl;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error: WSAStartup failed" << std::endl;
        return 1;
    }
#endif

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error: Could not create socket." << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons((unsigned short)port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error: Failed to bind to port " << port << std::endl;
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        std::cerr << "Error: Failed to listen on socket." << std::endl;
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    std::cout << "Server successfully started on port " << port << std::endl;
    std::cout << "Local Dashboard URL: http://localhost:" << port << "/" << std::endl;
    std::cout << "Awaiting HTTP requests..." << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

        if (clientSocket == INVALID_SOCKET) {
            continue;
        }

        // Read incoming request
        char buffer[8192];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string request(buffer);

            // Handle HTTP 100-Continue (required for .NET / PowerShell Invoke-RestMethod)
            if (request.find("100-continue") != std::string::npos ||
                request.find("100-Continue") != std::string::npos) {
                const char* contHeader = "HTTP/1.1 100 Continue\r\n\r\n";
                send(clientSocket, contHeader, (int)strlen(contHeader), 0);
            }

            // Parse HTTP method and path
            std::istringstream reqStream(request);
            std::string method, fullPath, httpVersion;
            if (!(reqStream >> method >> fullPath)) {
                closesocket(clientSocket);
                continue;
            }

            // Separate path and query string
            std::string path = fullPath;
            size_t qPos = fullPath.find('?');
            if (qPos != std::string::npos) {
                path = fullPath.substr(0, qPos);
            }

            // Extract initial body from current buffer
            std::string body = "";
            size_t headerEnd = request.find("\r\n\r\n");
            size_t delimLen = 4;
            if (headerEnd == std::string::npos) {
                headerEnd = request.find("\n\n");
                delimLen = 2;
            }
            if (headerEnd != std::string::npos) {
                body = request.substr(headerEnd + delimLen);
            }

            // Check Content-Length to ensure complete body is read
            size_t clPos = request.find("Content-Length:");
            if (clPos == std::string::npos) clPos = request.find("content-length:");
            int contentLength = 0;
            if (clPos != std::string::npos) {
                size_t lineEnd = request.find("\r\n", clPos);
                if (lineEnd == std::string::npos) lineEnd = request.find("\n", clPos);
                if (lineEnd != std::string::npos) {
                    std::string clStr = request.substr(clPos + 15, lineEnd - (clPos + 15));
                    contentLength = std::atoi(clStr.c_str());
                }
            }

            // If more body bytes are pending, read them from socket
            while ((int)body.length() < contentLength) {
                char extraBuf[4096];
                int n = recv(clientSocket, extraBuf, sizeof(extraBuf) - 1, 0);
                if (n <= 0) break;
                extraBuf[n] = '\0';
                body.append(extraBuf, n);
            }

            // Handle OPTIONS preflight for CORS
            if (method == "OPTIONS") {
                sendHttpResponse(clientSocket, 204, "No Content", "text/plain", "");
            }
            // Health checks for Render, Railway, Kubernetes
            else if (method == "GET" && (path == "/health" || path == "/healthz")) {
                sendHttpResponse(clientSocket, 200, "OK", "text/plain", "OK");
            }
            // Handle favicon
            else if (method == "GET" && path == "/favicon.ico") {
                sendHttpResponse(clientSocket, 204, "No Content", "image/x-icon", "");
            }
            // Handle HEAD request
            else if (method == "HEAD") {
                std::string fileContent;
                std::string contentType;
                std::string filePath = (path == "/" ? "/index.html" : path);
                if (readStaticFile(filePath, fileContent, contentType)) {
                    sendHttpResponse(clientSocket, 200, "OK", contentType, fileContent, false);
                } else {
                    sendHttpResponse(clientSocket, 200, "OK", "text/html", "", false);
                }
            }
            // 1. GET /api/websites
            else if (method == "GET" && path == "/api/websites") {
                sendHttpResponse(clientSocket, 200, "OK", "application/json", getWebsitesJson());
            }
            // 2. GET /api/state
            else if (method == "GET" && path == "/api/state") {
                sendHttpResponse(clientSocket, 200, "OK", "application/json", buildStateJson(true, "Current browser state"));
            }
            // 3. POST /api/visit
            else if (method == "POST" && path == "/api/visit") {
                std::string url = extractJsonField(body, "url");
                if (url.empty()) {
                    url = extractQueryParam(fullPath, "url");
                }

                if (url.empty()) {
                    sendHttpResponse(clientSocket, 400, "Bad Request", "application/json",
                                     "{\"success\": false, \"message\": \"Missing 'url' parameter in request body.\"}");
                } else {
                    g_browser.visitPage(url);
                    std::cout << "[VISIT] New page visited: " << url << std::endl;
                    sendHttpResponse(clientSocket, 200, "OK", "application/json",
                                     buildStateJson(true, "Successfully visited: " + url));
                }
            }
            // 4. POST /api/back
            else if (method == "POST" && path == "/api/back") {
                std::string msg;
                bool backSuccess = g_browser.goBack(msg);
                if (backSuccess) {
                    std::cout << "[BACK] Navigated back. " << msg << std::endl;
                    sendHttpResponse(clientSocket, 200, "OK", "application/json", buildStateJson(true, msg));
                } else {
                    std::cout << "[BACK WARN] " << msg << std::endl;
                    sendHttpResponse(clientSocket, 200, "OK", "application/json", buildStateJson(false, msg));
                }
            }
            // 5. POST /api/clear
            else if (method == "POST" && path == "/api/clear") {
                g_browser.clearHistory();
                std::cout << "[CLEAR] Browser history stack reset." << std::endl;
                sendHttpResponse(clientSocket, 200, "OK", "application/json",
                                 buildStateJson(true, "History stack cleared and current page reset."));
            }
            // 6. Static files serving (HTML, CSS, JS)
            else if (method == "GET") {
                std::string filePath = (path == "/" ? "/index.html" : path);
                std::string fileContent;
                std::string contentType;

                if (readStaticFile(filePath, fileContent, contentType)) {
                    sendHttpResponse(clientSocket, 200, "OK", contentType, fileContent);
                } else {
                    sendHttpResponse(clientSocket, 404, "Not Found", "text/plain", "404 Not Found");
                }
            } else {
                sendHttpResponse(clientSocket, 405, "Method Not Allowed", "text/plain", "Method Not Allowed");
            }
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
