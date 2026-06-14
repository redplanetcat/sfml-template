@echo off

setlocal
set "VISUAL_STUDIO_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build"
call "%VISUAL_STUDIO_PATH%\vcvarsall.bat" x64
devenv debug\main_win32.exe
endlocal
