@echo off
cd /d "%~dp0"

echo ==========================================
echo   Pushing ESP32 Sketch to GitHub...
echo ==========================================

:: Check if this folder is already a Git repository
if not exist ".git" (
    echo [ERROR] This folder is not a Git repository yet!
    echo Run 'git init' and add your remote origin first.
    pause
    exit /b
)

:: Stage all modified, new, and deleted files
git add .

:: Ask for a short commit note (defaults to timestamp if left blank)
set /p commit_msg="Enter commit message (or press Enter for auto-timestamp): "
if "%commit_msg%"=="" (
    set commit_msg=Update sketch - %date% %time%
)

:: Commit and push to GitHub
git commit -m "%commit_msg%"
git push

echo.
echo ==========================================
echo   Done!
echo ==========================================
pause
