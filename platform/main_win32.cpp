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

internal void
Win32GetExecutableFilename(win32_state* State) {
    DWORD SizeOfFilename = GetModuleFilename(0, State->ExeFilename, sizeof(State->ExeFilename));
    State->BasePath = State->ExeFilename;
    for (char* Scan = State->ExeFilename; *Scan; ++Scan) {
        if (*Scan == '\\') {
            State->BasePath = Scan + 1;
        }
    }
}

internal void
Win32BuildExecutablePathFilename(win32_state* State, const char* Filename,
                                 size_t DestCount, char* Dest)
{
    CatStrings((size_t)(State->BasePath - State->ExeFilename),
               State->ExeFilename,
               StringLength(Filename), Filename,
               DestCount, Dest);
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

  PlatformWindow.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Template");
  PlatformWindow.setFramerateLimit(60);

  triangle = sf::CircleShape{ 50.f, 3u };
  triangle.setFillColor({ 100u, 50u, 200u });
  triangle.setOrigin({ std::round(triangle.getLocalBounds().width / 2.f), std::round(triangle.getLocalBounds().height / 2.f) });
  triangle.setPosition({ WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f });

  while (PlatformWindow.isOpen()) {
    FILETIME NewDLLWriteTime = Win32GetLastWrieTime(SourceGameCodeDLLFullPath);
    if (CompareFileTime(&NewDLLWriteTime, Game.DLLLastWriteTime)) {
      Game.DLLLastWriteTime = NewDLLWriteTime;
      Win32UnloadGameCode(&Game);
      Game = Win32LoadGameCode(SourceGameCodeDLLFullPath,
                               TempGameCodeDLLFullPath);
    }
    if (Game.Update) {
      Game.Update(&GameMemory, &GameInput);
    }
    //PlatformHandleInput();
    PlatformWindow.clear();
    if (Game.Render) {
      Game.Render(&GameMemory);
    }
    //PlatformRenderTriangle();
    PlatformWindow.display();
  }
  return 0;
}

