@echo off

setlocal

set "PLATFORM_SRC_DIR=platform"
set "GAME_SRC_DIR=game"
set "COMMON_SRC_DIR=common"
set "BUILD_DIR=debug"
set "PLATFORM_OUT_NAME=main_win32.exe"
set "GAME_OUT_NAME=libgame.dll"
set "SFML_INCLUDE=..\SFML\include"
set "SFML_LIB=..\SFML\lib"

setlocal enabledelayedexpansion

set "PLATFORM_SRC_CHANGED=0"
set "GAME_SRC_CHANGED=0"

for /f "delims=" %%F in ('xcopy "%PLATFORM_SRC_DIR%\*.cpp" "%BUILD_DIR%\%PLATFORM_OUT_NAME%" /D /L /Y 2^>nul') do (
  echo %%F | findstr /R /C:"^[0-9]* File(s)" >nul || set "PLATFORM_SRC_CHANGED=1"
)
for /f "delims=" %%F in ('xcopy "%PLATFORM_SRC_DIR%\*.h" "%BUILD_DIR%\%PLATFORM_OUT_NAME%" /D /L /Y 2^>nul') do (
  echo %%F | findstr /R /C:"^[0-9]* File(s)" >nul || set "PLATFORM_SRC_CHANGED=1"
)
for /f "delims=" %%F in ('xcopy "%GAME_SRC_DIR%\*.cpp" "%BUILD_DIR%\%GAME_OUT_NAME%" /D /L /Y 2^>nul') do (
  echo %%F | findstr /R /C:"^[0-9]* File(s)" >nul || set "GAME_SRC_CHANGED=1"
)
for /f "delims=" %%F in ('xcopy "%GAME_SRC_DIR%\*.h" "%BUILD_DIR%\%GAME_OUT_NAME%" /D /L /Y 2^>nul') do (
  echo %%F | findstr /R /C:"^[0-9]* File(s)" >nul || set "GAME_SRC_CHANGED=1"
)
for /f "delims=" %%F in ('xcopy "%COMMON_SRC_DIR%\*.cpp" "%BUILD_DIR%\%PLATFORM_OUT_NAME%" /D /L /Y 2^>nul') do (
  echo %%F | findstr /R /C:"^[0-9]* File(s)" >nul || set "PLATFORM_SRC_CHANGED=1" || set "GAME_SRC_CHANGED=1"
)
for /f "delims=" %%F in ('xcopy "%COMMON_SRC_DIR%\*.h" "%BUILD_DIR%\%PLATFORM_OUT_NAME%" /D /L /Y 2^>nul') do (
  echo %%F | findstr /R /C:"^[0-9]* File(s)" >nul || set "PLATFORM_SRC_CHANGED=1" || set "GAME_SRC_CHANGED=1"
)

if "%PLATFORM_SRC_CHANGED%" == "0" if "%GAME_SRC_CHANGED%" == "0" (
  echo Build is up to date.
  goto :BuildEnd
)

echo "Building project..."

set "VISUAL_STUDIO_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build"
call "%VISUAL_STUDIO_PATH%\vcvarsall.bat" x64

if not exist "%BUILD_DIR%" (mkdir "%BUILD_DIR%")
pushd "%BUILD_DIR%"

set "SFML_LINK=sfml-window-s-d.lib sfml-graphics-s-d.lib sfml-audio-s-d.lib sfml-system-s-d.lib sfml-main-d.lib opengl32.lib gdi32.lib user32.lib winmm.lib advapi32.lib freetype.lib flac.lib ogg.lib openal32.lib vorbis.lib vorbisenc.lib vorbisfile.lib"

set "DEFINES=-DPLATFORM_WINDOWS=1 -DDEBUG=1 -DSFML_STATIC=1"

:: BUILD FLAGS
set FLAGS=        -nologo &:: Suppress startup banner
set FLAGS=%FLAGS% -FC     &:: Produce the full path of the source code file
set FLAGS=%FLAGS% -Z7     &:: Produce debug information
set FLAGS=%FLAGS% -Oi     &:: Use assembly intrinsics where possible
set FLAGS=%FLAGS% -MTd    &:: Include CRT library in the executable (static link)
set FLAGS=%FLAGS% -Od     &:: No optimizations (debug)
set FLAGS=%FLAGS% -Gm-    &:: Disable minimal rebuild
set FLAGS=%FLAGS% -GR-    &:: Disable RTTI (C++)
set FLAGS=%FLAGS% -EHa-   &:: Disable exception handling (C++)

:: WARNING LINK FLAGS
set WARNING_LINK_FLAGS=                     -W4     &:: Display warnings up to level 4
set WARNING_LINK_FLAGS=%WARNING_LINK_FLAGS% -WX     &:: Treat all warnings as errors
set WARNING_LINK_FLAGS=%WARNING_LINK_FLAGS% -wd4201 &:: Ignore nameless struct/union
set WARNING_LINK_FLAGS=%WARNING_LINK_FLAGS% -wd4100 &:: Ignore unused function parameter
set WARNING_LINK_FLAGS=%WARNING_LINK_FLAGS% -wd4189 &:: Ignore local variable not referenced
set WARNING_LINK_FLAGS=%WARNING_LINK_FLAGS% -wd4805 &:: Ignore bool operation warnings

:: COMMON LINKER SWITCHES
set WIN32_LINK=             -opt:ref              &:: Remove unused functions
set WIN32_LINK=%WIN32_LINK% -incremental:no       &:: Perform full link each time

:: DLL LINKER SWITCHES
set DLL_LINK=               /EXPORT:GameRender
set DLL_LINK=%DLL_LINK%     /EXPORT:GameUpdate

if "%PLATFORM_SRC_CHANGED%" == "1" (
  cl %DEFINES% %FLAGS% %WARNING_LINK_FLAGS% /I"..\%SFML_INCLUDE%" -Fm:"main_win32.map" "..\%PLATFORM_SRC_DIR%\main_win32.cpp" %SFML_LINK% /link /pdb:"main_win32.pdb" /LIBPATH:"..\%SFML_LIB%" %WIN32_LINK% -subsystem:windows,5.2 /OUT:"%PLATFORM_OUT_NAME%"
) else (
  echo Platform build is up to date.
)

if "%GAME_SRC_CHANGED%" == "1" (
  del *.pdb > NUL 2> NUL
  cl %DEFINES% %FLAGS% %WARNING_LINK_FLAGS% -Fm:"libgame.map" "..\%GAME_SRC_DIR%\game.cpp" -LD /link /pdb:"libgame%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb" %WIN32_LINK% %DLL_LINK% /OUT:"%GAME_OUT_NAME%"
) else (
  echo Game build is up to date.
)


popd

:BuildEnd

robocopy "%SFML_LIB%" "debug" *.dll >nul 2>&1
robocopy "Resources" "debug\Resources" /E /MIR >nul 2>&1
endlocal &::enabledelayedexpansion
endlocal &::PATH
