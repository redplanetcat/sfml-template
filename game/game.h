#if !defined(GAME_H)

#include "../common/defines.h"
#include "../common/types.h"


#define PLATFORM_RENDER_TRIANGLE(name) void name()
typedef PLATFORM_RENDER_TRIANGLE(platform_render_triangle_t);

#define PLATFORM_HANDLE_INPUT(name) void name()
typedef PLATFORM_HANDLE_INPUT(platform_handle_input_t);

struct game_input {

};

struct game_memory {
  u64 PermanentStorageSize;
  void* PermanentStorage;
  u64 ScratchStorageSize;
  void* ScratchStorage;
  b32 IsInitialized;

  platform_render_triangle_t* PlatformRenderTriangle;
  platform_handle_input_t* PlatformHandleInput;
};

struct game_state {

};

#define GAME_UPDATE(name) void name(game_memory* Memory, game_input* Input)
typedef GAME_UPDATE(game_update_t);
GAME_UPDATE(GameUpdateStub) { }

#define GAME_RENDER(name) void name(game_memory* Memory)
typedef GAME_RENDER(game_render_t);
GAME_RENDER(GameRenderStub) { }


#define GAME_H
#endif
