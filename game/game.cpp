#include "game.h"
#include <cstddef>
#include <cstring>
#include <cstdio>

vertex Vertices[MAX_VERTICES] = {};
u32 NumVertices = 0;

internal void
PushVertices(vertex* VerticesToPush, u32 NumVerticesToPush) {
  u32 NumVerticesToCopy = MIN(MAX_VERTICES-NumVertices, NumVerticesToPush);
  size_t BytesToCopy = (size_t)(NumVerticesToCopy * sizeof(Vertices[0]));
  vertex* VerticesPtr = &Vertices[NumVertices];
  memcpy((void*)VerticesPtr, (const void*)VerticesToPush, BytesToCopy);
  NumVertices += NumVerticesToCopy;
  printf("Pushed %u vertices: %lu bytes\n", NumVerticesToCopy, BytesToCopy);
}

internal void
GameReset(game_state* GameState) {
  GameState->Entities = entity_manager();

  vertex QuadVertices[6] = {
    {{ 0.f, 0.f }, { 0, 0, 255, 255 }, { -1.f, 0.f }},
    {{ 255.f, 0.f }, { 0, 255, 0, 255 }, { -1.f, 0.f }},
    {{ 255.f, 255.f }, { 255, 0, 0, 255 }, { -1.f, 0.f }},
    {{ 0.f, 0.f }, { 0, 0, 255, 255 }, { -1.f, 0.f }},
    {{ 0.f, 255.f }, { 255, 0, 255, 255 }, { -1.f, 0.f }},
    {{ 255.f, 255.f }, { 255, 0, 0, 255 }, { -1.f, 0.f }},
  };
  PushVertices(QuadVertices, 6);
}

internal void 
GameInit(game_state* GameState, platform_callbacks* Callbacks) {
  GameReset(GameState);
}

#if defined __cplusplus
extern "C"
#endif
GAME_UPDATE(GameUpdate) {
  game_state* GameState = (game_state*)Memory->PermanentStorage;
  if (!GameState->IsInitialized) {
    printf("GameState is uninitialized, calling init function.\n");
    GameInit(GameState, &Memory->PlatformCallbacks);
    GameState->IsInitialized = true;
  }
  Memory->PlatformCallbacks.PlatformHandleInput();
}

#if defined __cplusplus
extern "C"
#endif
GAME_RENDER(GameRender) {
  if (!Memory->IsInitialized) {

    Memory->IsInitialized = true;
  }
  game_state* GameState = (game_state*)Memory->PermanentStorage;
  Memory->PlatformCallbacks.PlatformRenderTriangle();
  Memory->PlatformCallbacks.PlatformDrawVertices(Vertices, NumVertices, primitive_type::Triangles);
}
