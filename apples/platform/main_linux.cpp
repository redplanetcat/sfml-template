#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <cstring>
#include <array>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "main_linux.h"
#include "platform_common.h"

#include "../common/strings.cpp"
#include "fileops_linux.cpp"
#include "memory_linux.cpp"
#include "platform_common.cpp"

internal linux_game_code
LinuxLoadGameCode(char* SourceSOName, char* TempSOName) {
  linux_game_code Result = {};

  if (LinuxCopyFile(SourceSOName, TempSOName) == 0) {
    printf("Loading game library...\n");
    Result.GameCodeSO = dlopen(SourceSOName, RTLD_LAZY);
    if (Result.GameCodeSO) {
      Result.Update = (game_update_t*)dlsym(Result.GameCodeSO, "GameUpdate");
      Result.Render = (game_render_t*)dlsym(Result.GameCodeSO, "GameRender");
      Result.Destroy = (game_destroy_t*)dlsym(Result.GameCodeSO, "GameDestroy");
      Result.IsValid = Result.Update && Result.Render && Result.Destroy;
      Result.SOLastWriteTime = LinuxGetLastWriteTime(SourceSOName);
      printf("Game dynamic library loaded successfully.\n");
    }
  } else {
    printf("Failed to copy %s to %s\n", SourceSOName, TempSOName);
  }

  if (!Result.IsValid) {
    printf("Failed to load game dynamic library.\n");
    Result.Update = 0;
    Result.Render = 0;
    Result.Destroy = 0;
  }

  return (Result);
}

internal void
LinuxUnloadGameCode(linux_game_code* GameCode) {
  if (GameCode->GameCodeSO) {
    if (dlclose(GameCode->GameCodeSO) != 0) {
      printf("%s\n", dlerror());
    }
    GameCode->GameCodeSO = 0;
  }
  GameCode->IsValid = false;
  GameCode->Update = 0;
  GameCode->Render = 0;
  GameCode->Destroy = 0;
}

int
main(int argc, char** argv) {
  LinuxGetExecutableFilename(&LinuxState);

  char SourceGameCodeSOFullPath[PLATFORM_FILENAME_COUNT];
  LinuxBuildExecutablePathFilename(&LinuxState, "libgame.so",
                                   sizeof(SourceGameCodeSOFullPath), SourceGameCodeSOFullPath);
  char TempGameCodeSOFullPath[PLATFORM_FILENAME_COUNT];
  LinuxBuildExecutablePathFilename(&LinuxState, "libgame_temp.so",
                                   sizeof(TempGameCodeSOFullPath), TempGameCodeSOFullPath);

  linux_game_code Game = LinuxLoadGameCode(SourceGameCodeSOFullPath, 
                                           TempGameCodeSOFullPath);

  game_memory GameMemory = {};
  if (AllocatePlatformMemory(&GameMemory) != 0) {
    printf("Failed to allocate memory on a host platform.");
    return 1;
  }
  AssignPlatformCallbacks(&GameMemory);

  game_input GameInput = game_input();

  platform_state& State = GetPlatformState();
  State.Window.setFramerateLimit(60);
  char DefaultFontPath[PLATFORM_FILENAME_COUNT];
  LinuxBuildExecutablePathFilename(&LinuxState, "Resources/Fonts/Roboto-Medium.ttf",
                                   sizeof(DefaultFontPath), DefaultFontPath);
  State.DefaultFont.loadFromFile(DefaultFontPath);

  sf::Clock DeltaClock;

  while (State.Window.isOpen()) {
    sf::Time DeltaTime = DeltaClock.restart();

    i64 NewSOWriteTime = LinuxGetLastWriteTime(SourceGameCodeSOFullPath);
    if (NewSOWriteTime != Game.SOLastWriteTime) {
      Game.SOLastWriteTime = NewSOWriteTime;
      LinuxUnloadGameCode(&Game);
      Game = LinuxLoadGameCode(SourceGameCodeSOFullPath,
                               TempGameCodeSOFullPath);
    }

    PlatformHandleInput(&GameInput);

    if (Game.Update) {
      Game.Update(&GameMemory, &GameInput, DeltaTime.asSeconds());
    }
    State.Window.clear();
    if (Game.Render) {
      Game.Render(&GameMemory);
    }
    State.Window.display();
  }
  if (Game.Destroy) {
    Game.Destroy(&GameMemory);
  }
  return 0;
}

#ifdef ARM_BUILD
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  void __stack_chk_fail(void) { abort(); }
  void __stack_chk_fail_local(void) { __stack_chk_fail(); }
  uintptr_t __stack_chk_guard = 0;
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // ARM_BUILD

