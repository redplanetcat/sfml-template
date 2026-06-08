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
#include <limits.h>
#include "main_linux.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


sf::CircleShape triangle;
sf::RenderWindow window;
sf::Event event;

inline i64
LinuxGetLastWriteTime(char* Filename) {
  struct stat FileStat = {};
  i64 LastWriteTime = 0;
  if (stat(Filename, &FileStat) == 0) {
    LastWriteTime = FileStat.st_mtim.tv_nsec;
  }
  return LastWriteTime;
}

internal void
CatStrings(size_t SourceACount, char* SourceA,
           size_t SourceBCount, char* SourceB,
           size_t DestCount, char* Dest) 
{
  for (size_t Index = 0; Index < SourceACount; ++Index) {
    *Dest++ = *SourceA++;
  }

  for (size_t Index = 0; Index < SourceBCount; ++Index) {
    *Dest++ = *SourceB++;
  }

  *Dest++ = 0;
}

internal int
LinuxCopyFile(char* src, char* dst) {
  int input_fd = open(src, O_RDONLY);
  if (input_fd < 0) return -1;

  int output_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (output_fd < 0) {
    close(input_fd);
    return -1;
  }

  struct stat stat_buf;
  if (fstat(input_fd, &stat_buf) < 0) {
    close(input_fd);
    close(output_fd);
    return -1;
  }

  off_t bytes_to_copy = stat_buf.st_size;
  loff_t total_copied = 0;

  while (bytes_to_copy > 0) {
    ssize_t ret = copy_file_range(input_fd, NULL, output_fd, NULL, (size_t)bytes_to_copy, 0);
    if (ret < 0) {
      close(input_fd);
      close(output_fd);
      return -1;
    }
    if (ret == 0) break;

    bytes_to_copy -= ret;
    total_copied += ret;
  }

  close(input_fd);
  close(output_fd);
  return 0;
}

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
    Result.GameUpdate = GameUpdateStub;
    Result.GameRender = GameRenderStub;
  }

  return (Result);
}

internal void
LinuxUnloadGameCode(linux_game_code* GameCode) {
  if (GameCode->GameCodeSO) {
    if (dlclose(GameCode->GameCodeSO) != 0) {
      fprintf(stderr, "%s\n", dlerror());
    }
    GameCode->GameCodeSO = 0;
  }
  GameCode->IsValid = false;
  GameCode->GameUpdate = GameUpdateStub;
  GameCode->GameRender = GameRenderStub;
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
  char BinFilename[PATH_MAX];
  ssize_t BinFilenameLength = readlink("/proc/self/exe", BinFilename, sizeof(BinFilename) - 1);
  if (BinFilenameLength != -1) {
    BinFilename[BinFilenameLength] = '\0';
  } else {
    perror("readlink");
  }
  
  char* OnePastLastSlash = BinFilename;
  for (char* Scan = BinFilename; *Scan; ++Scan) {
    if (*Scan == '/') {
      OnePastLastSlash = Scan + 1;
    }
  }

  char SourceGameCodeSOFilename[] = "libgame.so";
  char SourceGameCodeSOFullPath[PATH_MAX];

  CatStrings((size_t)(OnePastLastSlash - BinFilename), BinFilename,
             sizeof(SourceGameCodeSOFilename) - 1, SourceGameCodeSOFilename,
             sizeof(SourceGameCodeSOFullPath), SourceGameCodeSOFullPath);
  
  char TempGameCodeSOFilename[] = "libgame_temp.so";
  char TempGameCodeSOFullPath[PATH_MAX];

  CatStrings((size_t)(OnePastLastSlash - BinFilename), BinFilename,
             sizeof(TempGameCodeSOFilename) - 1, TempGameCodeSOFilename,
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
    Game.GameUpdate(&GameMemory, &GameInput);
    //PlatformHandleInput();
    window.clear();
    Game.GameRender(&GameMemory);
    //PlatformRenderTriangle();
    window.display();
  }
  return 0;
}

