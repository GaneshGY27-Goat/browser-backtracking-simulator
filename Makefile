# ==============================================================================
# Cross-Platform Makefile for Browser Backtracking Simulator
# Works on Windows (MinGW), Linux, and macOS
# ==============================================================================

CXX = g++
CXXFLAGS = -O2 -Wall -Wextra

ifeq ($(OS),Windows_NT)
    SERVER_TARGET = server.exe
    CONSOLE_TARGET = console_demo.exe
    LIBS = -lws2_32
    RM = del /Q /F
else
    SERVER_TARGET = server
    CONSOLE_TARGET = console_demo
    LIBS =
    RM = rm -f
endif

all: $(SERVER_TARGET) $(CONSOLE_TARGET)

$(SERVER_TARGET): server.cpp browser.cpp browser.h
	$(CXX) $(CXXFLAGS) server.cpp browser.cpp $(LIBS) -o $(SERVER_TARGET)

$(CONSOLE_TARGET): main_console.cpp browser.cpp browser.h
	$(CXX) $(CXXFLAGS) main_console.cpp browser.cpp -o $(CONSOLE_TARGET)

run: $(SERVER_TARGET)
	./$(SERVER_TARGET)

run-console: $(CONSOLE_TARGET)
	./$(CONSOLE_TARGET)

clean:
	$(RM) $(SERVER_TARGET) $(CONSOLE_TARGET) *.o

.PHONY: all run run-console clean
