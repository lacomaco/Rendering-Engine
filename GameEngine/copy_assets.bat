@echo off
set TARGET_DIR=%1
echo %TARGET_DIR%
powershell -ExecutionPolicy Bypass -File "E:\code\GameEngine\GameEngine\copy_assets.ps1" "%TARGET_DIR%"