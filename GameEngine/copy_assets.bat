@echo off
echo Copying assets folder...

REM ���� ��ũ��Ʈ�� ��ġ�� ���丮�� �̵�
cd /d "%~dp0"

REM ����� ��ġ�� ��θ� ���ڷ� ����
set TARGET_DIR=%1

REM assets ������ ����� ��ġ�� ����
xcopy "assets" "%TARGET_DIR%\assets" /E /I /Y

echo Assets copied successfully.