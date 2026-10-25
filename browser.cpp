#include "browser.h"

// Constructor: Initializes browser with no active page and empty stack
Browser::Browser() {
    currentPage = "";
}

// 1. visitPage(string url):
// If currentPage is not empty, push it onto the history stack.
// Then make the selected URL the current page.
void Browser::visitPage(const std::string& url) {
    if (!currentPage.empty()) {
        history.push(currentPage); // Push old page onto stack
    }
    currentPage = url; // Update current active page
}

// 2. goBack():
// If the stack is not empty, restore previous page using top() and pop().
// Otherwise, indicate that no previous page is available.
bool Browser::goBack(std::string& message) {
    if (history.empty()) {
        message = "No previous page available in history stack.";
        return false;
    }

    // Top element of the stack contains the most recently visited previous page
    std::string previousPage = history.top();
    history.pop(); // Remove it from stack as we are navigating back to it

    currentPage = previousPage; // Restore previous page as current page
    message = "Navigated back to: " + currentPage;
    return true;
}

// 3. showCurrentPage():
// Return the current page URL.
std::string Browser::showCurrentPage() const {
    return currentPage;
}

// 4. showHistory():
// Return the previous pages in most-recent-first order without modifying the original stack.
// We make a copy of the stack and pop from the copy.
std::vector<std::string> Browser::showHistory() const {
    std::vector<std::string> historyList;
    std::stack<std::string> tempStack = history; // Copy stack to preserve original state

    while (!tempStack.empty()) {
        historyList.push_back(tempStack.top()); // Most recent is at the top
        tempStack.pop();                        // Remove from temporary copy
    }

    return historyList;
}

// 5. clearHistory():
// Clear the stack and reset the current page.
void Browser::clearHistory() {
    while (!history.empty()) {
        history.pop();
    }
    currentPage = "";
}

// Helper: Check if history stack is empty
bool Browser::isHistoryEmpty() const {
    return history.empty();
}

// Helper: Get number of items in history stack
size_t Browser::getHistorySize() const {
    return history.size();
}
