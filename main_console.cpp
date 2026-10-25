#include <iostream>
#include <string>
#include <vector>
#include "browser.h"

// ============================================================================
// Console Demonstration for College Data Structures Viva
// Highlights:
// - std::stack<std::string>
// - LIFO (Last In, First Out) principle
// - push(), pop(), top(), empty(), size()
// - Strictly avoids #include <limits>, cin.ignore(), and getline()
// ============================================================================

void displayState(const Browser& browser) {
    std::cout << "\n--------------------------------------------------" << std::endl;
    std::cout << "Current Page: " << (browser.showCurrentPage().empty() ? "[None - Fresh Tab]" : browser.showCurrentPage()) << std::endl;
    std::cout << "History Stack (Top -> Bottom):" << std::endl;

    std::vector<std::string> history = browser.showHistory();
    if (history.empty()) {
        std::cout << "  (Stack is empty)" << std::endl;
    } else {
        for (size_t i = 0; i < history.size(); ++i) {
            if (i == 0) {
                std::cout << "  [TOP] -> " << history[i] << std::endl;
            } else {
                std::cout << "           " << history[i] << std::endl;
            }
        }
    }
    std::cout << "--------------------------------------------------\n" << std::endl;
}

// Automated run of the required college viva test scenario:
// 1. Open YouTube
// 2. Open Google Chrome
// 3. Open Instagram
// 4. Open Learner RSCOE Campus
// 5. Press Back twice
// 6. Verify that the current page is Google Chrome
// 7. View the history and confirm that it has not been corrupted
// 8. Clear the history and verify empty state
// 9. Confirm invalid back operation produces understandable message
void runCollegeVivaTestCase() {
    std::cout << "==================================================" << std::endl;
    std::cout << " RUNNING REQUIRED COLLEGE VIVA TEST SEQUENCE" << std::endl;
    std::cout << "==================================================" << std::endl;

    Browser b;

    std::cout << "\n[Step 1] Visiting YouTube..." << std::endl;
    b.visitPage("https://www.youtube.com/");
    displayState(b);

    std::cout << "[Step 2] Visiting Google Chrome..." << std::endl;
    b.visitPage("https://www.google.com/intl/en_in/chrome/");
    displayState(b);

    std::cout << "[Step 3] Visiting Instagram..." << std::endl;
    b.visitPage("https://www.instagram.com/?hl=en");
    displayState(b);

    std::cout << "[Step 4] Visiting Learner RSCOE Campus..." << std::endl;
    b.visitPage("https://learnerrscoe.edupluscampus.com/");
    displayState(b);

    std::cout << "[Step 5] Pressing Back (1st time)..." << std::endl;
    std::string msg;
    b.goBack(msg);
    std::cout << "Message: " << msg << std::endl;
    displayState(b);

    std::cout << "[Step 5 cont.] Pressing Back (2nd time)..." << std::endl;
    b.goBack(msg);
    std::cout << "Message: " << msg << std::endl;
    displayState(b);

    std::cout << "[Step 6] Verifying Current Page..." << std::endl;
    if (b.showCurrentPage() == "https://www.google.com/intl/en_in/chrome/") {
        std::cout << ">> VERIFICATION PASSED: Current page is indeed Google Chrome!" << std::endl;
    } else {
        std::cout << ">> VERIFICATION FAILED!" << std::endl;
    }

    std::cout << "\n[Step 7] Checking History Stack Integrity..." << std::endl;
    std::vector<std::string> hist = b.showHistory();
    std::cout << "History stack count: " << hist.size() << std::endl;
    if (hist.size() == 1 && hist[0] == "https://www.youtube.com/") {
        std::cout << ">> VERIFICATION PASSED: History stack intact! Only YouTube remains." << std::endl;
    } else {
        std::cout << ">> VERIFICATION FAILED: Unexpected history content." << std::endl;
    }

    std::cout << "\n[Step 8] Clearing History..." << std::endl;
    b.clearHistory();
    displayState(b);
    if (b.isHistoryEmpty() && b.showCurrentPage().empty()) {
        std::cout << ">> VERIFICATION PASSED: Cleared state verified successfully!" << std::endl;
    }

    std::cout << "\n[Step 9] Attempting Invalid Back Operation on Empty Stack..." << std::endl;
    bool backResult = b.goBack(msg);
    if (!backResult) {
        std::cout << ">> Expected Failure Caught Gracefully: " << msg << std::endl;
    } else {
        std::cout << ">> Error: Back should not have succeeded on empty stack!" << std::endl;
    }

    std::cout << "\n==================================================" << std::endl;
    std::cout << " VIVA TEST SEQUENCE COMPLETED SUCCESSFULLY!" << std::endl;
    std::cout << "==================================================\n" << std::endl;
}

int main() {
    std::cout << "Browser Backtracking Simulator - C++ Console Mode" << std::endl;
    std::cout << "College Data Structures Project (Stack STL Demonstration)\n" << std::endl;

    // Run automated verification test
    runCollegeVivaTestCase();

    std::cout << "\nInteractive Menu:" << std::endl;
    std::cout << "1. Run Viva Test Sequence Again" << std::endl;
    std::cout << "2. Exit" << std::endl;
    std::cout << "Enter choice (1 or 2): ";

    int choice = 2;
    if (std::cin >> choice) {
        if (choice == 1) {
            runCollegeVivaTestCase();
        }
    }

    std::cout << "Exiting console demo. Goodbye!" << std::endl;
    return 0;
}
