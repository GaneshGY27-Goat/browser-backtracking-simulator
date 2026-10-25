@echo off
echo ===========================================================================
echo       Push Browser Backtracking Simulator to GitHub (GaneshGY27-GOAT)
echo ===========================================================================
echo.
echo Make sure you have created an empty repository on GitHub named:
echo    browser-backtracking-simulator
echo at: https://github.com/new
echo.
pause
echo.
echo Configuring remote and pushing to GitHub...
git remote remove origin 2>nul
git remote add origin https://github.com/GaneshGY27-GOAT/browser-backtracking-simulator.git
git branch -M main
git push -u origin main
echo.
if errorlevel 1 (
    echo [ERROR] Git push failed. Please check your GitHub credentials or ensure the repo exists.
) else (
    echo [SUCCESS] Code pushed successfully to:
    echo https://github.com/GaneshGY27-GOAT/browser-backtracking-simulator
    echo.
    echo Now open Render (https://dashboard.render.com), click "New + Web Service",
    echo select this repository, and click "Deploy"!
)
echo.
pause
