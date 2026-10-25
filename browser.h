#ifndef BROWSER_H
#define BROWSER_H

#include <string>
#include <stack>
#include <vector>

// ============================================================================
// Class: Browser
// Description: Core navigation logic using standard C++ Stack (std::stack)
// Data Structure Concept: LIFO (Last In, First Out)
// ============================================================================
class Browser {
private:
    std::stack<std::string> history; // Stack storing previous URLs
    std::string currentPage;         // Active website URL

public:
    // Constructor initializes an empty browser state
    Browser();

    // 1. Visit a new page:
    //    If a current page exists, push it onto the history stack.
    //    Then set currentPage to the new URL.
    void visitPage(const std::string& url);

    // 2. Go back to previous page:
    //    If the stack is not empty, restore the previous page using top() and pop().
    //    Otherwise, return false with a message stating no previous page exists.
    bool goBack(std::string& message);

    // 3. Show current active page
    std::string showCurrentPage() const;

    // 4. Show history in most-recent-first order without modifying original stack
    std::vector<std::string> showHistory() const;

    // 5. Clear history stack and reset current page
    void clearHistory();

    // Helper utilities for stack inspection
    bool isHistoryEmpty() const;
    size_t getHistorySize() const;
};

#endif // BROWSER_H
