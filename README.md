# Browser Backtracking Simulator

> **College Data Structures & Algorithms Mini Project**  
> **Core Topic:** Stack Data Structure (LIFO - Last In, First Out)  
> **Language:** C++ (Standard Template Library `std::stack<std::string>`)  
> **Frontend:** Vanilla HTML5, Modern CSS3, JavaScript (ES6)  
> **Backend:** Lightweight, Cross-Platform C++ HTTP Server (Winsock2 / POSIX Sockets)

---

## 1. Project Overview

The **Browser Backtracking Simulator** is an interactive, full-stack educational tool designed to demonstrate how real-world web browsers implement the **Back Button** mechanism using a **Stack** data structure.

Every time a user visits a new website:
1. The currently active page is **pushed** onto a history stack.
2. The new website becomes the active page.
3. When the user clicks **Back**, the most recently visited page is retrieved from the **top** of the stack and **popped**, restoring previous state.

This project implements the core navigation engine in **pure, beginner-friendly C++**, exposing clean REST API endpoints to a modern, responsive web dashboard with a live visual stack visualizer.

---

## 2. Data Structure Concept: LIFO Stack

A **Stack** is a linear data structure that adheres strictly to the **LIFO (Last In, First Out)** principle: the last element inserted is the first element to be removed.

### Core Stack Operations in C++ STL (`std::stack<std::string>`):

| Operation | C++ Function | Time Complexity | Description in Browser Context |
| :--- | :--- | :--- | :--- |
| **Push** | `history.push(currentPage)` | $O(1)$ | Stores the active URL into history before switching to a new site. |
| **Pop** | `history.pop()` | $O(1)$ | Removes the top URL from history when navigating backward. |
| **Top** | `history.top()` | $O(1)$ | Reads the most recent previous URL to restore as the active page. |
| **Empty Check** | `history.empty()` | $O(1)$ | Verifies if backward navigation is possible (prevents Stack Underflow). |
| **Size** | `history.size()` | $O(1)$ | Returns total count of previous pages stored. |

### Educational Stack Traversal (Non-Destructive Inspection)
In C++, `std::stack` does not provide an iterator by design. To inspect the history stack without corrupting the original data:
```cpp
std::vector<std::string> Browser::showHistory() const {
    std::vector<std::string> historyList;
    std::stack<std::string> tempStack = history; // Make a copy of the stack

    while (!tempStack.empty()) {
        historyList.push_back(tempStack.top()); // Read top (most recent)
        tempStack.pop();                        // Remove from temporary copy
    }
    return historyList; // Original stack remains completely untouched!
}
```

---

## 3. Predefined Websites

The project includes four predefined college-approved websites displayed as interactive cards:

| Name | URL | Brand / Category |
| :--- | :--- | :--- |
| **YouTube** | `https://www.youtube.com/` | Video Streaming Platform |
| **Google Chrome** | `https://www.google.com/intl/en_in/chrome/` | Web Browser Portal |
| **Instagram** | `https://www.instagram.com/?hl=en` | Social Media Platform |
| **Learner RSCOE Campus**| `https://learnerrscoe.edupluscampus.com/` | College Academic Campus Portal |

---

## 4. Project File Structure

```text
DSA_Project/
├── browser.h           # Browser class declaration (stack<string> history, string currentPage)
├── browser.cpp         # Implementation of visitPage, goBack, showCurrentPage, showHistory, clearHistory
├── main_console.cpp    # Standalone terminal/console demo for college viva test cases
├── server.cpp          # Lightweight C++ HTTP REST API server & static file host (port 8080)
├── build_and_run.bat   # Windows one-click compilation and launcher script
├── Makefile            # Linux / macOS build script
├── test_sequence.ps1   # Automated PowerShell test suite for viva sequence
├── Dockerfile          # Multi-stage production container build for cloud hosting
├── render.yaml         # Blueprint for one-click deployment on Render.com
├── public/             # Modern frontend dashboard
│   ├── index.html      # Accessible semantic HTML5 layout with stack visualizer
│   ├── style.css       # Clean dark slate theme with glassmorphism & stack animations
│   └── app.js          # JavaScript client communicating with C++ backend
└── README.md           # Comprehensive project documentation & viva guide
```

---

## 5. Local Setup & Running Instructions

### Method A: One-Click Windows Script (Easiest)
Simply double-click `build_and_run.bat` or run in PowerShell:
```powershell
.\build_and_run.bat
```
Select:
- `[1]` to compile and start the Web Server (automatically opens `http://localhost:8080/` in your browser).
- `[2]` to run the Console Viva Demo in the terminal.
- `[3]` to run the Automated PowerShell Test Suite.

---

### Method B: Manual Compilation (Windows MinGW GCC)

1. **Compile and Run the Web Server:**
   ```powershell
   g++ -O2 server.cpp browser.cpp -lws2_32 -o server.exe
   .\server.exe
   ```
   Open your browser and navigate to: **`http://localhost:8080/`**

2. **Compile and Run the Console Demo:**
   ```powershell
   g++ -O2 main_console.cpp browser.cpp -o console_demo.exe
   .\console_demo.exe
   ```

---

### Method C: Manual Compilation (Linux / macOS)

1. **Compile with Makefile:**
   ```bash
   make
   ```
2. **Run Web Server:**
   ```bash
   ./server
   ```
3. **Run Console Demo:**
   ```bash
   ./console_demo
   ```

---

## 6. REST API Endpoints

The C++ backend provides 5 clean JSON endpoints:

### 1. `GET /api/websites`
Returns the 4 predefined websites.
```json
{
  "websites": [
    { "id": "youtube", "name": "YouTube", "url": "https://www.youtube.com/" },
    { "id": "chrome", "name": "Google Chrome", "url": "https://www.google.com/intl/en_in/chrome/" },
    { "id": "instagram", "name": "Instagram", "url": "https://www.instagram.com/?hl=en" },
    { "id": "rscoe", "name": "Learner RSCOE Campus", "url": "https://learnerrscoe.edupluscampus.com/" }
  ]
}
```

### 2. `GET /api/state`
Returns active webpage and history stack.
```json
{
  "success": true,
  "currentPage": "https://www.google.com/intl/en_in/chrome/",
  "historySize": 1,
  "canGoBack": true,
  "history": [
    "https://www.youtube.com/"
  ]
}
```

### 3. `POST /api/visit`
Visits a selected URL and pushes previous page to stack.
- **Request Body:** `{"url": "https://www.youtube.com/"}`
- **Response:**
```json
{
  "success": true,
  "message": "Successfully visited: https://www.youtube.com/",
  "currentPage": "https://www.youtube.com/",
  "historySize": 0,
  "canGoBack": false,
  "history": []
}
```

### 4. `POST /api/back`
Pops the previous page from stack and restores it as the current page.
- **Response (Success):**
```json
{
  "success": true,
  "message": "Navigated back to: https://www.google.com/intl/en_in/chrome/",
  "currentPage": "https://www.google.com/intl/en_in/chrome/",
  "historySize": 1,
  "canGoBack": true,
  "history": ["https://www.youtube.com/"]
}
```
- **Response (When Stack is Empty):**
```json
{
  "success": false,
  "message": "No previous page available in history stack.",
  "currentPage": "",
  "historySize": 0,
  "canGoBack": false,
  "history": []
}
```

### 5. `POST /api/clear`
Clears history stack and resets active page.
```json
{
  "success": true,
  "message": "History stack cleared and current page reset.",
  "currentPage": "",
  "historySize": 0,
  "canGoBack": false,
  "history": []
}
```

---

## 7. College Viva Test Scenario Verification

The following standard test sequence is verified automatically by `test_sequence.ps1` and `main_console.cpp`:

| Step | Action Taken | Current Page | Stack Size | Stack Contents (`[TOP]` to Bottom) | Verification Status |
| :---: | :--- | :--- | :---: | :--- | :---: |
| **1** | Open YouTube | YouTube | 0 | *(Empty - First page opened)* | **PASS** |
| **2** | Open Google Chrome | Google Chrome | 1 | `[TOP] -> YouTube` | **PASS** |
| **3** | Open Instagram | Instagram | 2 | `[TOP] -> Chrome` <br> `YouTube` | **PASS** |
| **4** | Open Learner RSCOE | Learner RSCOE | 3 | `[TOP] -> Instagram` <br> `Chrome` <br> `YouTube` | **PASS** |
| **5a** | Press Back (1st time) | Instagram | 2 | `[TOP] -> Chrome` <br> `YouTube` | **PASS** |
| **5b** | Press Back (2nd time) | **Google Chrome** | 1 | `[TOP] -> YouTube` | **PASS** |
| **6** | Verify Active Page | **Google Chrome** | 1 | Restored to Google Chrome | **PASS** |
| **7** | Verify History Stack | Google Chrome | 1 | YouTube remains at top | **PASS** |
| **8** | Clear History | *[Empty]* | 0 | *(Empty)* | **PASS** |
| **9** | Press Back on Empty Stack | *[Empty]* | 0 | Returns graceful message: *"No previous page available..."* | **PASS** |

---

## 8. Important Notice: Browser Tab Isolation Boundary

In actual web browsers, **tab isolation** and the **Same-Origin Policy** are core security fundamentals. 
- A web application running inside one tab is strictly forbidden by browser security specifications from forcefully hijacking native browser buttons or controlling external tabs (e.g., forcing an Instagram or YouTube tab to navigate backwards).
- Therefore, in this simulator:
  1. Clicking **Open Website** opens the target website in a fresh tab for genuine user browsing (`window.open(url, '_blank')`).
  2. Simultaneously, the simulator sends the URL to the C++ backend so the stack state and current-page model update.
  3. Clicking **Back** updates the internal C++ stack and displays the restored URL in the simulator dashboard.
- This accurately reflects how browser engineers structure internal navigation data structures.

---

## 9. Universal Shareable Link & Cloud Deployment Guide

To share a single public HTTPS URL with your professor and classmates, follow these step-by-step instructions using **Render** (which supports free C++ Docker web services):

### Step 1: Push Code to GitHub
1. Initialize git and push this project folder to your GitHub account:
   ```bash
   git init
   git add .
   git commit -m "Browser Backtracking Simulator C++ Mini Project"
   git branch -M main
   git remote add origin https://github.com/<your-username>/browser-backtracking-simulator.git
   git push -u origin main
   ```

### Step 2: Deploy to Render.com (Free Tier)
1. Sign up / Log in to [render.com](https://render.com) using your GitHub account.
2. Click **New +** in the top navigation bar and select **Web Service**.
3. Choose **Build and deploy from a Git repository** and connect your `browser-backtracking-simulator` repo.
4. Set the following settings:
   - **Name:** `browser-backtracking-simulator`
   - **Region:** Choose the closest region (e.g., Singapore or Oregon).
   - **Language / Runtime:** **Docker** (Render will automatically detect the provided `Dockerfile`).
   - **Instance Type:** **Free**.
5. Click **Deploy Web Service**.

### Step 3: Deployment Process
Render will:
- Pull the repository.
- Use Docker to compile `server.cpp` and `browser.cpp` using `g++ -O3`.
- Bundle the static `public/` frontend.
- Launch the binary listening on port `$PORT` (assigned by Render).
- Provision a free SSL certificate.

### Step 4: Obtain Your Universal Shareable Link
Once the build completes (typically 2-3 minutes), Render displays your live universal URL:
```text
https://browser-backtracking-simulator.onrender.com/
```
Both the C++ backend API and the interactive HTML/CSS/JS frontend are served from this single HTTPS origin with zero configuration needed.

---

## 10. Common College Viva Questions & Answers

### Q1: Why is a Stack chosen for browser back navigation instead of a Queue or Array?
> **Answer:** Browser backtracking requires **LIFO (Last In, First Out)** behavior. The website visited most recently must be the first one returned to when pressing "Back". A Queue operates on **FIFO (First In, First Out)**, which would return the oldest visited page first, which is incorrect for backtracking. While an array or dynamic array can simulate a stack, the `std::stack` container adapter provides strict encapsulation with guaranteed $O(1)$ operations for `push()`, `pop()`, and `top()`.

### Q2: How does the Forward button work in real browsers?
> **Answer:** Real browsers use **two stacks**: a **Back Stack** and a **Forward Stack**. When you visit a new page, the forward stack is cleared. When you click **Back**, the current page is pushed onto the Forward Stack while popping from the Back Stack. When you click **Forward**, the reverse operation occurs.

### Q3: What is Stack Underflow and how is it handled here?
> **Answer:** Stack Underflow occurs when an attempt is made to pop or read the top of an empty stack (`history.empty() == true`). In our C++ `Browser::goBack()` implementation, we check `if (history.empty())` before calling `top()` or `pop()`, returning an informative message without crashing.

### Q4: What is the Space and Time Complexity of this project?
> **Answer:**
> - **Time Complexity:** $O(1)$ for each `visitPage()`, `goBack()`, and `showCurrentPage()`. $O(N)$ for `showHistory()` because it traverses $N$ items to format the view.
> - **Space Complexity:** $O(N)$ where $N$ is the number of visited URLs stored in `history`.

---

## 11. Author & Acknowledgments

- **Subject:** Data Structures & Algorithms (DSA Practical Mini Project)
- **Institution:** Department of Computer Engineering / IT, RSCOE Pune
- **Libraries Used:** C++ Standard Library (`<stack>`, `<string>`, `<vector>`, `<iostream>`), Winsock2 (Windows) / BSD Sockets (Linux).
