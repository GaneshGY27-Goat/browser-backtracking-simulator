@echo off
REM ===========================================================================
REM Browser Backtracking Simulator - Quick Build and Run Script for Windows
REM ===========================================================================
setlocal EnableDelayedExpansion

echo ===========================================================================
echo       Browser Backtracking Simulator - C++ DSA Mini Project
echo ===========================================================================
echo.
echo Choose an option:
echo  [1] Compile and Start Local Web Server (http://localhost:8080)
echo  [2] Start Server with Instant Public HTTPS Tunnel (Universal Shareable Link)
echo  [3] Compile and Run Console Viva Demo (Automated Test Cases)
echo  [4] Run Automated API Test Sequence (PowerShell)
echo  [5] Compile All C++ Binaries (server.exe and console_demo.exe)
echo  [6] Exit
echo.
set /p choice="Enter choice (1-6): "

if "%choice%"=="1" goto run_server
if "%choice%"=="2" goto run_tunnel
if "%choice%"=="3" goto run_console
if "%choice%"=="4" goto run_test
if "%choice%"=="5" goto compile_all
if "%choice%"=="6" goto end

echo Invalid option selected.
pause
goto end

:run_server
echo.
echo Compiling C++ HTTP Server...
g++ -O2 server.cpp browser.cpp -lws2_32 -o server.exe
if errorlevel 1 (
    echo [ERROR] Compilation failed! Ensure MinGW g++ is in your PATH.
    pause
    goto end
)
echo [SUCCESS] Server compiled successfully.
echo Starting C++ Backend on http://localhost:8080/ ...
echo Opening browser...
start http://localhost:8080/
server.exe
goto end

:run_tunnel
echo.
echo Compiling C++ HTTP Server...
g++ -O2 server.cpp browser.cpp -lws2_32 -o server.exe
if errorlevel 1 (
    echo [ERROR] Compilation failed!
    pause
    goto end
)
echo [SUCCESS] Starting C++ Backend in background...
start /B server.exe
timeout /t 2 /nobreak >nul
echo.
echo ===========================================================================
echo Creating Direct Public HTTPS Link (Cloudflare Quick Tunnel - No Sign-in)
echo ===========================================================================
echo Look for the https://*.trycloudflare.com link below.
echo Anyone around the world can open it directly with zero passwords or sign-in.
echo.
npx.cmd -y cloudflared tunnel --url http://localhost:8080
goto end

:run_console
echo.
echo Compiling Console Viva Demo...
g++ -O2 main_console.cpp browser.cpp -o console_demo.exe
if errorlevel 1 (
    echo [ERROR] Compilation failed!
    pause
    goto end
)
echo [SUCCESS] Running Console Demo...
console_demo.exe
pause
goto end

:run_test
echo.
echo Running Automated API Test Sequence...
powershell -ExecutionPolicy Bypass -File test_sequence.ps1
pause
goto end

:compile_all
echo.
echo Compiling server.exe...
g++ -O2 server.cpp browser.cpp -lws2_32 -o server.exe
if errorlevel 1 (
    echo [ERROR] Failed to compile server.exe
) else (
    echo [OK] server.exe built.
)

echo Compiling console_demo.exe...
g++ -O2 main_console.cpp browser.cpp -o console_demo.exe
if errorlevel 1 (
    echo [ERROR] Failed to compile console_demo.exe
) else (
    echo [OK] console_demo.exe built.
)
pause
goto end

:end
