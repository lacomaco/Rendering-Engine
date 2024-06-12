@echo off
echo Copying assets folder...

REM 현재 스크립트가 위치한 디렉토리로 이동
cd /d "%~dp0"

REM 빌드된 위치의 경로를 인자로 받음
set TARGET_DIR=%1

REM assets 폴더를 빌드된 위치로 복사
xcopy "assets" "%TARGET_DIR%\assets" /E /I /Y

echo Assets copied successfully.