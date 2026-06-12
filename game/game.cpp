#include "game.h"
#include <cstddef>

internal void
CreatePlayer(game_state* GameState) {
  GameState->PlayerRef = GameState->Entities.Add(kind::Player);
  printf("Created player handle: %i\n", GameState->PlayerRef.Idx);
  entity& Player = GameState->Entities.Get(GameState->PlayerRef);
  Player.Pos = { WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f };
  Player.Size = { 36.f, 36.f };
  Player.Color = { 0, 0, 255, 255 };
  Player.Flags |= entity_flags::Drawable;
}

internal void
PushVertices(game_state* GameState, vertex* VerticesToPush, u32 NumVerticesToPush) {
  u32 NumVerticesToCopy = MIN(MAX_VERTICES-GameState->NumVertices, NumVerticesToPush);
  size_t BytesToCopy = (size_t)(NumVerticesToCopy * sizeof(VerticesToPush[0]));
  vertex* VerticesPtr = &GameState->Vertices[GameState->NumVertices];
  memcpy((void*)VerticesPtr, (const void*)VerticesToPush, BytesToCopy);
  GameState->NumVertices += NumVerticesToCopy;
}

internal void
GameReset(game_state* GameState) {
  GameState->Entities = entity_manager();
  CreatePlayer(GameState);
}

internal void 
GameInit(game_state* GameState, platform_callbacks* Callbacks) {
  GameReset(GameState);
} 

struct player_settings {
  f32 Speed = 200.f;
};

inline constexpr player_settings PlayerSettings = player_settings();

internal void
UpdatePlayer(game_state* GameState, game_input* Input, f32 Delta) {
  entity& Player = GameState->Entities.Get(GameState->PlayerRef);
  if (Player) {
    vec2 Dir = vec2{
      f32(Input->IsKeyDown(key::D)) - f32(Input->IsKeyDown(key::A)),
      f32(Input->IsKeyDown(key::S)) - f32(Input->IsKeyDown(key::W))
    };
    Player.Pos += Dir * PlayerSettings.Speed * Delta;
  }
}

internal void
UpdateCollisions(game_state* GameState) {

}

internal void
UpdateEntityGraphics(game_state* GameState) {
  entity_manager& EM = GameState->Entities;
  for (const entity& E : EM) {
    if ((E.Flags & entity_flags::Drawable) == entity_flags::Drawable) {
      vertex Quad[6];
      MakeQuad(Quad, E.Pos.x - E.Size.x/2.f, E.Pos.y - E.Size.y/2.f,
               E.Size.x, E.Size.y, E.Color);
      PushVertices(GameState, Quad, 6);
    }
  }
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

  UpdatePlayer(GameState, Input, Delta);
  UpdateCollisions(GameState);
  UpdateEntityGraphics(GameState);

  Input->SwapStates();
}

#if defined __cplusplus
extern "C"
#endif
GAME_RENDER(GameRender) {
  if (!Memory->IsInitialized) {

    Memory->IsInitialized = true;
  }
  game_state* GameState = (game_state*)Memory->PermanentStorage;
  Memory->PlatformCallbacks.PlatformDrawVertices(GameState->Vertices, 
                                                 GameState->NumVertices, 
                                                 primitive_type::Triangles);
  GameState->NumVertices = 0;
}
