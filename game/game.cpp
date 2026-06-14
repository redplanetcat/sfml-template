#include "game.h"
#include <cstddef>
#include <ctime>

struct player_settings {
  f32 InitialSpeed = 200.f;
  f32 SpeedDelta = 16.f;
  vec2 Size = vec2{36.f, 36.f};
  vec2 Collider = vec2{36.f, 36.f};
  color Color = color{ 0, 0, 255, 255 };
  vec2 StartPosition = vec2{ WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f };
  f32 RestartTime = 3.f;
  i32 AppleScore = 100;
};

constexpr player_settings PlayerSettings = player_settings();

internal void 
MakeQuad(vertex* v, f32 x, f32 y, f32 width, f32 height, color c) {
  v[0] = {{x, y}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[1] = {{x+width, y}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[2] = {{x+width, y+height}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[3] = {{x, y}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[4] = {{x, y+height}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[5] = {{x+width, y+height}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
}

internal void
CreatePlayer(game_state* GameState) {
  GameState->PlayerRef = GameState->Entities.Add(kind::Player);
  printf("Created player handle: %i\n", GameState->PlayerRef.Idx);
  entity& Player = GameState->Entities.Get(GameState->PlayerRef);
  Player.Pos = PlayerSettings.StartPosition;
  Player.Size = PlayerSettings.Size;
  Player.Collider = PlayerSettings.Collider;
  Player.Color = PlayerSettings.Color;
  Player.Flags |= entity_flags::Drawable;
}

internal vec2
GetRandomPointAwayFromPlayer(game_state* GameState, f32 Radius) {
  vec2 PlayerPosition = vec2{WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f};
  entity& Player = GameState->Entities.Get(GameState->PlayerRef);
  if (Player) {
    PlayerPosition = Player.Pos;
  }
  f32 RadiusSq = Radius * Radius;
  vec2 RandomPosition = {0.f, 0.f};

  do {
    RandomPosition = vec2{
      ((f32)rand() / (f32)RAND_MAX) * (WINDOW_WIDTH-64.f) + 32.f,
      ((f32)rand() / (f32)RAND_MAX) * (WINDOW_HEIGHT-64.f) + 32.f
    };
  } while (PlayerPosition.DistanceToSq(RandomPosition) < RadiusSq);
  return RandomPosition;
}

internal void
SpawnApple(game_state* GameState) {
  vec2 ApplePosition = GetRandomPointAwayFromPlayer(GameState, 64);
  entity_ref AppleRef = GameState->Entities.Add(kind::Apple);
  printf("Created apple handle: %i\n", AppleRef.Idx);
  entity& Apple = GameState->Entities.Get(AppleRef);
  Apple.Pos = ApplePosition;
  Apple.Size = vec2{16.f, 16.f};
  Apple.Collider = Apple.Size;
  Apple.Color = color{255, 0, 0, 255};
  Apple.Timer = 0.5f;
  Apple.TimerLength = 0.5f;
  Apple.Flags |= entity_flags::Drawable;
}

internal void
SpawnStone(game_state* GameState) {
  vec2 StonePosition = GetRandomPointAwayFromPlayer(GameState, 64);
  entity_ref StoneRef = GameState->Entities.Add(kind::Stone);
  printf("Created stone handle: %i\n", StoneRef.Idx);
  entity& Stone = GameState->Entities.Get(StoneRef);
  Stone.Pos = StonePosition;
  Stone.Size = vec2{32.f, 32.f};
  Stone.Collider = Stone.Size;
  Stone.Color = color{100, 100, 100, 255};
  Stone.Timer = 0.5f;
  Stone.TimerLength = 0.5f;
  Stone.Flags |= entity_flags::Drawable;
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
UpdateScore(game_state* GameState, platform_callbacks* Callbacks, u32 Score) {
  GameState->Score = Score;
  char ScoreText[128];
  snprintf(ScoreText, sizeof(ScoreText), "Score: %u", GameState->Score);
  Callbacks->PlatformUpdateText(GameState->ScoreTextHandle,
                                (const char*)ScoreText,
                                color{255, 0, 0, 255},
                                vec2{16.f, 16.f},
                                vec2{1.f, 1.f},
                                rect_alignment::TopLeft);
}

internal void
GameReset(game_state* GameState, platform_callbacks* Callbacks) {
  UpdateScore(GameState, Callbacks, 0);
  GameState->Entities = entity_manager();
  CreatePlayer(GameState);
  GameState->PlayerSpeed = PlayerSettings.InitialSpeed;
  srand((unsigned int)time(NULL));
  SpawnApple(GameState);
}

internal void 
GameInit(game_state* GameState, platform_callbacks* Callbacks) {
  if (GameState->BackgroundShaderHandle == 0) {
    GameState->BackgroundShaderHandle = Callbacks->PlatformLoadShader("Resources/Shaders/background.vert", "Resources/Shaders/background.frag");
    printf("Background shader handle: %u\n", GameState->BackgroundShaderHandle);
  }
  if (GameState->ScoreTextHandle == 0) {
    GameState->ScoreTextHandle = Callbacks->PlatformCreateText("Score: 0", 16,
                                                               color{255, 0, 0, 255});
    Callbacks->PlatformUpdateText(GameState->ScoreTextHandle,
                                  "Score:0",
                                  color{255, 0, 0, 255},
                                  vec2{16.f, 16.f},
                                  vec2{1.f, 1.f},
                                  rect_alignment::TopLeft);
  }
  if (GameState->RestartTextHandle == 0) {
    GameState->RestartTextHandle = Callbacks->PlatformCreateText("3", 20,
                                                                 color{255, 0, 0, 255});
    Callbacks->PlatformUpdateText(GameState->RestartTextHandle,
                                  "3",
                                  color{255, 0, 0, 255},
                                  vec2{WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f},
                                  vec2{3.f, 3.f},
                                  rect_alignment::Center);
  }
  GameReset(GameState, Callbacks);
} 

internal void
DrawBackground(game_state* GameState, platform_callbacks* Callbacks) {
  vec2 Position = vec2{-WINDOW_WIDTH/2.f, -WINDOW_HEIGHT/2.f};
  entity& Player = GameState->Entities.Get(GameState->PlayerRef);
  if (Player) {
    Position = Player.Pos;
  }
  vertex ScreenQuad[6];
  MakeQuad(ScreenQuad, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {255, 255, 255, 255});
  Callbacks->PlatformUseShader(GameState->BackgroundShaderHandle);
  Callbacks->PlatformSetShaderUniformVec2(GameState->BackgroundShaderHandle,
                                          "u_resolution",
                                          vec2{WINDOW_WIDTH, WINDOW_HEIGHT});
  Callbacks->PlatformSetShaderUniformVec2(GameState->BackgroundShaderHandle,
                                          "u_position",
                                          Position);
  Callbacks->PlatformDrawVertices(ScreenQuad, 6, primitive_type::Triangles);
  Callbacks->PlatformUseShader(0);
}

internal void
DrawText(game_state* GameState, platform_callbacks* Callbacks) {
  Callbacks->PlatformDrawText(GameState->ScoreTextHandle);
  if (GameState->RestartTimer > 0.f) {
    Callbacks->PlatformDrawText(GameState->RestartTextHandle);
  }
}

internal void
OnPlayerDied(game_state* GameState, platform_callbacks* Callbacks) {
  entity_manager& EM = GameState->Entities;
  entity& Player = EM.Get(GameState->PlayerRef);
  if (Player) {
    EM.Rem(GameState->PlayerRef);
  }
  GameState->RestartTimer = PlayerSettings.RestartTime;
  printf("Player died! Timer: %f\n", GameState->RestartTimer);
}

internal void
UpdatePlayer(game_state* GameState, 
             platform_callbacks* Callbacks, 
             game_input* Input, 
             f32 Delta) 
{
  entity& Player = GameState->Entities.Get(GameState->PlayerRef);
  if (Player) {
    if (Input->IsKeyDown(key::D)) {
      Player.Dir = vec2{1.f, 0.f};
    } else if (Input->IsKeyDown(key::A)) {
      Player.Dir = vec2{-1.f, 0.f};
    } else if (Input->IsKeyDown(key::S)) {
      Player.Dir = vec2{0.f, 1.f};
    } else if (Input->IsKeyDown(key::W)) {
      Player.Dir = vec2{0.f, -1.f};
    }
    Player.Pos += Player.Dir * GameState->PlayerSpeed * Delta;

    rect PlayerRect = rect{
      Player.Pos.x - Player.Collider.x/2.f,
      Player.Pos.y - Player.Collider.y/2.f,
      Player.Collider.x,
      Player.Collider.y
    };
    rect LevelRect = rect{
      0, 0,
      WINDOW_WIDTH,
      WINDOW_HEIGHT
    };
    if (!LevelRect.Contains(PlayerRect)) {
      printf("Out of bounds death!\n");
      OnPlayerDied(GameState, Callbacks);
    }
  } else if (GameState->RestartTimer <= 0.f) {
    GameReset(GameState, Callbacks);
  }

}

internal void
OnAppleCollect(entity_ref AppleRef, game_state* GameState, platform_callbacks* Callbacks) {
  entity_manager& EM = GameState->Entities;
  EM.Rem(AppleRef);
  printf("Apple collected!\n");
  UpdateScore(GameState, Callbacks, GameState->Score + PlayerSettings.AppleScore);
  GameState->PlayerSpeed += PlayerSettings.SpeedDelta;
  SpawnApple(GameState);
  if (GameState->Score % 1000 == 0) {
    SpawnStone(GameState);
  }
}

internal void
UpdateCollisions(game_state* GameState, platform_callbacks* Callbacks) {
  entity_manager& EM = GameState->Entities;
  entity& Player = EM.Get(GameState->PlayerRef);
  if (!Player) {
    return;
  }
  rect PlayerRect = rect{
    Player.Pos.x - Player.Collider.x/2.f,
    Player.Pos.y - Player.Collider.y/2.f,
    Player.Collider.x,
    Player.Collider.y
  };
  for (auto EIter = EM.begin(); EIter != EM.end(); ++EIter) {
    if (EIter->Kind == kind::Apple) {
      rect AppleRect = rect{
        EIter->Pos.x - EIter->Collider.x/2.f,
        EIter->Pos.y - EIter->Collider.y/2.f,
        EIter->Collider.x,
        EIter->Collider.y
      };
      if (PlayerRect.Overlaps(AppleRect)) {
        OnAppleCollect(EIter.Ref, GameState, Callbacks);
      }
    } else if (EIter->Kind == kind::Stone) {
      rect StoneRect = rect{
        EIter->Pos.x - EIter->Collider.x/2.f,
        EIter->Pos.y - EIter->Collider.y/2.f,
        EIter->Collider.x,
        EIter->Collider.y
      };
      if (PlayerRect.Overlaps(StoneRect)) {
        printf("Stoned death!\n");
        OnPlayerDied(GameState, Callbacks);
      }
    }
  }
}

internal void
UpdateEntityGraphics(game_state* GameState) {
  entity_manager& EM = GameState->Entities;
  for (const entity& E : EM) {
    if ((E.Flags & entity_flags::Drawable) == entity_flags::Drawable) {
      vertex Quad[6];
      vec2 ScaledSize = vec2{ E.Size.x * (1.f + E.Scale), E.Size.y * (1.f + E.Scale) };
      MakeQuad(Quad, E.Pos.x - ScaledSize.x/2.f, E.Pos.y - ScaledSize.y/2.f,
               ScaledSize.x, ScaledSize.y, E.Color);
      PushVertices(GameState, Quad, 6);
    }
  }
}

internal void
UpdateTimers(game_state* GameState, platform_callbacks* Callbacks, f32 Delta) {
  if (GameState->RestartTimer > 0.f) {
    GameState->RestartTimer = MAX(0.f, GameState->RestartTimer - Delta);
    i32 DecimalPart = (i32)GameState->RestartTimer;
    char RestartText[16];
    snprintf(RestartText, sizeof(RestartText), "%i", DecimalPart+1);
    f32 Fraction = GameState->RestartTimer - (f32)DecimalPart;
    f32 Scale = Fraction * 3.f;
    Callbacks->PlatformUpdateText(GameState->RestartTextHandle,
                                  (const char*)RestartText,
                                  color{255, 0, 0, 255},
                                  vec2{WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f},
                                  vec2{Scale, Scale},
                                  rect_alignment::Center);
  }
}

internal void
UpdateApplesAndStones(game_state* GameState, f32 Delta) {
  entity_manager& EM = GameState->Entities;
  for (auto E = EM.begin(); E != EM.end(); ++E) {
    if ((E->Kind == kind::Apple) || (E->Kind == kind::Stone)) {
      if (E->Timer > 0.f) {
        E->Timer = MAX(0.f, E->Timer - Delta);
        f32 Progress = (E->TimerLength - E->Timer)/E->TimerLength;
        E->Scale = f32(sin(Progress*PI32));
      }
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

  UpdateTimers(GameState, &Memory->PlatformCallbacks, Delta);
  UpdatePlayer(GameState, &Memory->PlatformCallbacks, Input, Delta);
  UpdateApplesAndStones(GameState, Delta);
  UpdateCollisions(GameState, &Memory->PlatformCallbacks);
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
  DrawBackground(GameState, &Memory->PlatformCallbacks);
  Memory->PlatformCallbacks.PlatformDrawVertices(GameState->Vertices, 
                                                 GameState->NumVertices, 
                                                 primitive_type::Triangles);
  GameState->NumVertices = 0;
  DrawText(GameState, &Memory->PlatformCallbacks);
}
