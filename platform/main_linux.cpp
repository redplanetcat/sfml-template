#include <SFML/Graphics.hpp>
#include <cmath>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "main_linux.h"
#include "fileops_linux.cpp"
#include "../common/strings.cpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


sf::CircleShape triangle;
sf::RenderWindow window;
sf::Event event;

internal linux_game_code
LinuxLoadGameCode(char* SourceSOName, char* TempSOName) {
  linux_game_code Result = {};

  if (LinuxCopyFile(SourceSOName, TempSOName) == 0) {
    printf("Loading game library...\n");
    Result.GameCodeSO = dlopen(SourceSOName, RTLD_LAZY);
    if (Result.GameCodeSO) {
      Result.GameUpdate = (game_update_t*)dlsym(Result.GameCodeSO, "GameUpdate");
      Result.GameRender = (game_render_t*)dlsym(Result.GameCodeSO, "GameRender");
      Result.IsValid = Result.GameUpdate && Result.GameRender;
      Result.SOLastWriteTime = LinuxGetLastWriteTime(SourceSOName);
      printf("Game dynamic library loaded successfully.\n");
    }
  } else {
    printf("Failed to copy %s to %s\n", SourceSOName, TempSOName);
  }

  if (!Result.IsValid) {
    printf("Failed to load game dynamic library.\n");
    Result.GameUpdate = 0;
    Result.GameRender = 0;
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
  GameCode->GameUpdate = 0;
  GameCode->GameRender = 0;
}

internal void
LinuxGetExecutableFilename(linux_state* State) {
  ssize_t BinFilenameLength = readlink("/proc/self/exe", State->BinFilename, sizeof(State->BinFilename) - 1);
  if (BinFilenameLength != -1) {
    State->BinFilename[BinFilenameLength] = '\0';
  } else {
    perror("readlink");
    return;
  }
  
  State->BasePath = State->BinFilename;
  for (char* Scan = State->BinFilename; *Scan; ++Scan) {
    if (*Scan == '/') {
      State->BasePath = Scan + 1;
    }
  }
}

internal void
LinuxBuildExecutablePathFilename(linux_state* State, const char* Filename,
                                 size_t DestCount, char* Dest)
{
  CatStrings((size_t)(State->BasePath - State->BinFilename), 
             State->BinFilename,
             StringLength(Filename), Filename,
             DestCount, Dest);
}

PLATFORM_HANDLE_INPUT(PlatformHandleInput) {
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    } else if (event.type == sf::Event::MouseMoved) {
      auto mouse_position = window.mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });
      auto delta_position = mouse_position - triangle.getPosition();
      float rotation = RAD2DEG((float)std::atan2(delta_position.y, delta_position.x)) + 90.f;

      triangle.setRotation(rotation);
    }
  }
}

PLATFORM_RENDER_TRIANGLE(PlatformRenderTriangle) {
  window.draw(triangle);
}

int
main(int _argc, char** _argv) {
  linux_state LinuxState = {};
  LinuxGetExecutableFilename(&LinuxState);

  char SourceGameCodeSOFullPath[PATH_MAX];
  LinuxBuildExecutablePathFilename(&LinuxState, "libgame.so",
                                   sizeof(SourceGameCodeSOFullPath), SourceGameCodeSOFullPath);
  char TempGameCodeSOFullPath[PATH_MAX];
  LinuxBuildExecutablePathFilename(&LinuxState, "libgame_temp.so",
                                   sizeof(TempGameCodeSOFullPath), TempGameCodeSOFullPath);
  
  linux_game_code Game = LinuxLoadGameCode(SourceGameCodeSOFullPath, 
                                           TempGameCodeSOFullPath);

  game_memory GameMemory = {};
  GameMemory.PermanentStorageSize = Megabytes(16);
  GameMemory.ScratchStorageSize = Megabytes(16);
  GameMemory.PermanentStorage = mmap(NULL, GameMemory.PermanentStorageSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (GameMemory.PermanentStorage == MAP_FAILED) {
    printf("Failed to reserve permanent memory of %lu bytes.\n", GameMemory.PermanentStorageSize);
    return 1;
  } else {
    int result = mprotect(GameMemory.PermanentStorage, GameMemory.PermanentStorageSize, PROT_READ | PROT_WRITE);
    if (result == -1) {
      printf("Failed to commit permanent memory of %lu bytes.\n", GameMemory.PermanentStorageSize);
      return 1;
    }
  }
  GameMemory.ScratchStorage = mmap(NULL, GameMemory.ScratchStorageSize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (GameMemory.ScratchStorage == MAP_FAILED) {
    printf("Failed to reserve scratch memory of %lu bytes.\n", GameMemory.ScratchStorageSize);
    return 1;
  } else {
    int result = mprotect(GameMemory.ScratchStorage, GameMemory.ScratchStorageSize, PROT_READ | PROT_WRITE);
    if (result == -1) {
      printf("Failed to commit scratch memory of %lu bytes.\n", GameMemory.ScratchStorageSize);
      return 1;
    }
  }
  GameMemory.PlatformHandleInput = PlatformHandleInput;
  GameMemory.PlatformRenderTriangle = PlatformRenderTriangle;

  game_input GameInput = { };

  window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Template");
  window.setFramerateLimit(60);

  triangle = sf::CircleShape{ 50.f, 3u };
  triangle.setFillColor({ 100u, 50u, 200u });
  triangle.setOrigin({ std::round(triangle.getLocalBounds().width / 2.f), std::round(triangle.getLocalBounds().height / 2.f) });
  triangle.setPosition({ WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f });

  while (window.isOpen()) {
    i64 NewSOWriteTime = LinuxGetLastWriteTime(SourceGameCodeSOFullPath);
    if (NewSOWriteTime != Game.SOLastWriteTime) {
      Game.SOLastWriteTime = NewSOWriteTime;
      LinuxUnloadGameCode(&Game);
      Game = LinuxLoadGameCode(SourceGameCodeSOFullPath,
                               TempGameCodeSOFullPath);
    }
    if (Game.GameUpdate) {
      Game.GameUpdate(&GameMemory, &GameInput);
    }
    //PlatformHandleInput();
    window.clear();
    if (Game.GameRender) {
      Game.GameRender(&GameMemory);
    }
    //PlatformRenderTriangle();
    window.display();
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

