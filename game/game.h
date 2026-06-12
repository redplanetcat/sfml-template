#ifndef GAME_H

#include <cstring>
#include <cstdio>
#include "../common/defines.h"
#include "../common/types.h"
#include <cmath>
#include "../common/linalg.h"
#include "graphics.h"
#include "entities.h"
#include "input.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define PLATFORM_DRAW_VERTICES(name) void name(vertex* Vertices, u32 NumVertices, \
                                               primitive_type PrimitiveType)
typedef PLATFORM_DRAW_VERTICES(platform_draw_vertices_t);

struct platform_callbacks {
  platform_draw_vertices_t* PlatformDrawVertices;
};

struct game_memory {
  u64 PermanentStorageSize;
  void* PermanentStorage;
  u64 ScratchStorageSize;
  void* ScratchStorage;
  b32 IsInitialized;
  platform_callbacks PlatformCallbacks;
};

struct game_state {
  entity_manager Entities;
  entity_ref PlayerRef;
  u32 SortedEntities[MAX_ENTITIES];
  vertex Vertices[MAX_VERTICES];
  u32 NumVertices;
  b32 IsInitialized;
};

#define GAME_UPDATE(name) void name(game_memory* Memory, game_input* Input, f32 Delta)
typedef GAME_UPDATE(game_update_t);

#define GAME_RENDER(name) void name(game_memory* Memory)
typedef GAME_RENDER(game_render_t);


#define GAME_H
#endif
