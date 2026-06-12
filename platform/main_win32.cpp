#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstring>
#include <array>
#include "../common/strings.cpp"
#include "main_win32.h"
#include "fileops_win32.cpp"
#include "platform_common.cpp"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define VC_EXTRALEAN
#include <windows.h>
#include "memory_win32.cpp"

internal win32_game_code
Win32LoadGameCode(char* SourceDLLName, char* TempDLLName) {
    win32_game_code Result = {};
    CopyFile(SourceDLLName, TempDLLName, FALSE);
    Result.GameCodeDLL = LoadLibraryA("libgame.dll");
    if (Result.GameCodeDLL) {
        Result.Update = (game_update_t*)GetProcAddress(Result.GameCodeDLL, "GameUpdate");
        Result.Render = (game_render_t*)GetProcAddress(Result.GameCodeDLL, "GameRender");
        Result.IsValid = Result.Update && Result.Render;
        Result.DLLLastWriteTime = Win32GetLastWriteTime(SourceDLLName);
        printf("Game dynamic library loaded successfully.\n");
    }

    if (!Result.IsValid) {
        printf("Failed to load game dynamic library.\n");
        GameCode->Update = 0;
        GameCode->Render = 0;
    }

    return (Result);
}

internal void
Win32UnloadGameCode(win32_game_code* GameCode) {
    if (GameCode->GameCodeDLL) {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }

    GameCode->IsValid = false;
    GameCode->Update = 0;
    GameCode->Render = 0;
}

int main(int argc, char** argv) {
    win32_state Win32State = {};
    Win32GetExecutableFilename(&Win32State);

    char SourceGameCodeDLLFullPath[MAX_PATH];
    Win32BuildExecutablePathFilename(&Win32State, "libgame.dll",
                                     sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath);
    char TempGameCodeDLLFullPath[MAX_PATH];
    Win32BuildExecutablePathFilename(&Win32State, "libgame_temp.dll",
                                     sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath);

    win32_game_code Game = Win32LoadGameCode(SourceGameCodeDLLFullPath,
                                             TempGameCodeDLLFullPath);

    game_memory GameMemory = {};
    if (AllocatePlatformMemory(&GameMemory) != 0) {
        printf("Failed to allocate memory on a host platform.");
        return 1;
    }
    AssignPlatformCallacks(&GameMemory);

    game_input GameInput = game_input();

  PlatformWindow.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Template");
  PlatformWindow.setFramerateLimit(60);

  sf::Clock DeltaClock;

  while (PlatformWindow.isOpen()) {
    sf::Time DeltaTime = DeltaClock.restart();

    FILETIME NewDLLWriteTime = Win32GetLastWrieTime(SourceGameCodeDLLFullPath);
    if (CompareFileTime(&NewDLLWriteTime, Game.DLLLastWriteTime)) {
      Game.DLLLastWriteTime = NewDLLWriteTime;
      Win32UnloadGameCode(&Game);
      Game = Win32LoadGameCode(SourceGameCodeDLLFullPath,
                               TempGameCodeDLLFullPath);
    }

    PlatformHandleInput(&GameInput);

    if (Game.Update) {
      Game.Update(&GameMemory, &GameInput, DeltaTime.asSeconds());
    }
    PlatformWindow.clear();
    if (Game.Render) {
      Game.Render(&GameMemory);
    }
    PlatformWindow.display();
  }
  return 0;
}

