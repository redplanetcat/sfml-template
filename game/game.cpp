#include "game.h"
#include <cstddef>
#include <ctime>

struct player_settings {
  f32 InitialSpeed = 200.f;
  f32 SpeedDelta = 16.f;
  vec2 Size = vec2{36.f, 36.f};
  vec2 Collider = vec2{36.f, 36.f};
  color Color = color{ 255, 255, 255, 255 };
  vec2 StartPosition = vec2{ WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f };
  f32 RestartTime = 3.f;
  i32 AppleScore = 100;
};

constexpr player_settings PlayerSettings = player_settings();

internal vec2
RotatePoint(vec2 Position, vec2 Center, f32 Angle) {
  float TranslatedX = Position.x - Center.x;
  float TranslatedY = Position.y - Center.y;

  float RotatedX = TranslatedX * (f32)cos(Angle) - TranslatedY * (f32)sin(Angle);
  float RotatedY = TranslatedX * (f32)sin(Angle) + TranslatedY * (f32)cos(Angle);

  Position.x = RotatedX + Center.x;
  Position.y = RotatedY + Center.y;

  return Position;
}

internal void
RotateQuad(vertex* V, vec2 Center, f32 Rotation) {
  float RotationRad = Rotation * PI32 / 180.f;

  for (i32 I = 0; I < 6; ++I) {
    vertex* Vert = &V[I];
    Vert->Position = RotatePoint(Vert->Position, Center, RotationRad);
  }
}

internal void 
MakeQuad(vertex* V, f32 X, f32 Y, f32 Width, f32 Height, color C, f32 TexWidth, f32 TexHeight, f32 Rotation, b32 Flip) {
  f32 FlipWidth = (Flip ? TexWidth : 0.f);
  V[0] = {{X, Y}, { C.r, C.g, C.b, C.a }, {0.f + FlipWidth, 0.f}};
  V[1] = {{X+Width, Y}, { C.r, C.g, C.b, C.a }, {TexWidth - FlipWidth, 0.f}};
  V[2] = {{X+Width, Y+Height}, { C.r, C.g, C.b, C.a }, {TexWidth - FlipWidth, TexHeight}};
  V[3] = {{X, Y}, { C.r, C.g, C.b, C.a }, {0.f + FlipWidth, 0.f}};
  V[4] = {{X, Y+Height}, { C.r, C.g, C.b, C.a }, {0.f + FlipWidth, TexHeight}};
  V[5] = {{X+Width, Y+Height}, { C.r, C.g, C.b, C.a }, {TexWidth - FlipWidth, TexHeight}};

  RotateQuad(V, vec2{X + Width/2.f, Y + Height/2.f}, Rotation);

}

internal void
CreatePlayer(game_state* GameState) {
  GameState->PlayerRef = GameState->Entities.Add(kind::Player);
  entity& Player = GameState->Entities.Get(GameState->PlayerRef);
  Player.Pos = PlayerSettings.StartPosition;
  Player.Size = PlayerSettings.Size;
  Player.Collider = PlayerSettings.Collider;
  Player.Color = PlayerSettings.Color;
  Player.Texture = GameState->PacmanTextureHandle;
  Player.Flags |= (u32)entity_flags::Drawable;
  printf("Created player handle: %i with texture handle: %u [%f:%f]\n", GameState->PlayerRef.Idx, Player.Texture.Handle, Player.Texture.Width, Player.Texture.Height);
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
  entity& Apple = GameState->Entities.Get(AppleRef);
  Apple.Pos = ApplePosition;
  Apple.Size = vec2{16.f, 16.f};
  Apple.Collider = Apple.Size;
  Apple.Color = color{255, 255, 255, 255};
  Apple.Texture = GameState->AppleTextureHandle;
  Apple.Timer = 0.5f;
  Apple.TimerLength = 0.5f;
  Apple.Flags |= (u32)entity_flags::Drawable;
  printf("Created apple handle: %i with texture handle: %u [%f:%f]\n", AppleRef.Idx, Apple.Texture.Handle, Apple.Texture.Width, Apple.Texture.Height);
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
  Stone.Texture = GameState->RockTextureHandle;
  Stone.Timer = 0.5f;
  Stone.TimerLength = 0.5f;
  Stone.Flags |= (u32)entity_flags::Drawable;
}

internal void
PushVertices(vertex* VerticesDst, u32* DstNumVertices, u32 DstMaxVertices,
             vertex* VerticesToPush, u32 NumVerticesToPush) {
  u32 NumVerticesToCopy = MIN(DstMaxVertices - *DstNumVertices, NumVerticesToPush);
  size_t BytesToCopy = (size_t)(NumVerticesToCopy * sizeof(VerticesToPush[0]));
  vertex* VerticesPtr = &VerticesDst[*DstNumVertices];
  memcpy((void*)VerticesPtr, (const void*)VerticesToPush, BytesToCopy);
  *DstNumVertices += NumVerticesToCopy;
}

inline void
PushDrawCommand(game_state* GameState, draw_command* Command) {
  draw_command* DestCommand = (GameState->NumDrawCommands < MAX_DRAW_COMMANDS
                               ? &GameState->CommandBuffer[GameState->NumDrawCommands++]
                               : &GameState->CommandBuffer[0]);
  *DestCommand = *Command;
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
  if (GameState->BackgroundShaderHandle.Handle == 0) {
    GameState->BackgroundShaderHandle = Callbacks->PlatformLoadShader("Resources/Shaders/background.vert", "Resources/Shaders/background.frag");
    printf("Background shader handle: %u\n", GameState->BackgroundShaderHandle.Handle);
  }
  if (GameState->ScoreTextHandle.Handle == 0) {
    GameState->ScoreTextHandle = Callbacks->PlatformCreateText("Score: 0", 16,
                                                               color{255, 0, 0, 255});
    Callbacks->PlatformUpdateText(GameState->ScoreTextHandle,
                                  "Score:0",
                                  color{255, 0, 0, 255},
                                  vec2{16.f, 16.f},
                                  vec2{1.f, 1.f},
                                  rect_alignment::TopLeft);
  }
  if (GameState->RestartTextHandle.Handle == 0) {
    GameState->RestartTextHandle = Callbacks->PlatformCreateText("3", 20,
                                                                 color{255, 0, 0, 255});
    Callbacks->PlatformUpdateText(GameState->RestartTextHandle,
                                  "3",
                                  color{255, 0, 0, 255},
                                  vec2{WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f},
                                  vec2{3.f, 3.f},
                                  rect_alignment::Center);
  }
  if (GameState->AppleTextureHandle.Handle == 0) {
    GameState->AppleTextureHandle = Callbacks->PlatformLoadTexture("Resources/Textures/Apple.png");
    printf("Apple texture handle: %u\n", GameState->AppleTextureHandle.Handle);
  }
  if (GameState->RockTextureHandle.Handle == 0) {
    GameState->RockTextureHandle = Callbacks->PlatformLoadTexture("Resources/Textures/Rock.png");
    printf("Rock texture handle: %u\n", GameState->RockTextureHandle.Handle);
  }
  if (GameState->PacmanTextureHandle.Handle == 0) {
    GameState->PacmanTextureHandle = Callbacks->PlatformLoadTexture("Resources/Textures/Pacman.png");
    printf("Pacman texture handle: %u\n", GameState->PacmanTextureHandle.Handle);
  }
  if (GameState->CrunchSoundHandle.Handle == 0) {
    GameState->CrunchSoundHandle = Callbacks->PlatformLoadSound("Resources/Sounds/Crunch.ogg");
    printf("Crunch sound handle: %u\n", GameState->CrunchSoundHandle.Handle);
  }
  if (GameState->DeathSoundHandle.Handle == 0) {
    GameState->DeathSoundHandle = Callbacks->PlatformLoadSound("Resources/Sounds/Death.ogg");
    printf("Death sound handle: %u\n", GameState->DeathSoundHandle.Handle);
  }
  GameState->NumDrawCommands = 1;
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
  MakeQuad(ScreenQuad, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, {255, 255, 255, 255}, 0.f, 0.f, 0.f, false);
  Callbacks->PlatformUseShader(GameState->BackgroundShaderHandle);
  Callbacks->PlatformSetShaderUniformVec2(GameState->BackgroundShaderHandle,
                                          "u_resolution",
                                          vec2{WINDOW_WIDTH, WINDOW_HEIGHT});
  Callbacks->PlatformSetShaderUniformVec2(GameState->BackgroundShaderHandle,
                                          "u_position",
                                          Position);
  Callbacks->PlatformDrawVertices(ScreenQuad, 6, primitive_type::Triangles);
  Callbacks->PlatformUseShader(shader_id{});
}

internal int
CompareCommands(const void* A, const void* B) {
  draw_command* CmdA = (draw_command*)A;
  draw_command* CmdB = (draw_command*)B;
  u32 DrawStateA = 0, DrawStateB = 0; // 0000 ZZZZ SSSS TTTT
  DrawStateA |= (CmdA->Z << 16);
  DrawStateA |= (CmdA->Shader.Handle << 8);
  DrawStateA |= CmdA->Texture.Handle;
  DrawStateB |= (CmdB->Z << 16);
  DrawStateB |= (CmdB->Shader.Handle << 8);
  DrawStateB |= CmdB->Texture.Handle;
  return ((i32)DrawStateA - (i32)DrawStateB);
}

internal void
FlushCommandBuffer(game_state* GameState, platform_callbacks* Callbacks) {
  qsort(GameState->CommandBuffer+1, 
        GameState->NumDrawCommands-1, 
        sizeof(draw_command), 
        CompareCommands);
  texture_id Texture = { };
  shader_id Shader = { };
  vertex VertexBuffer[MAX_DRAW_COMMANDS * 6];
  u32 NumVertices = 0;
  b32 RenderStateChanged;
  for (u32 I = 1; I < GameState->NumDrawCommands; ++I) {
    draw_command* Command = &GameState->CommandBuffer[I];
    RenderStateChanged = ((Command->Shader.Handle != Shader.Handle)
                       || (Command->Texture.Handle != Texture.Handle));
    if (RenderStateChanged) {
      if (NumVertices > 0) {
        Callbacks->PlatformUseShader(Shader);
        Callbacks->PlatformUseTexture(Texture);
        Callbacks->PlatformDrawVertices(VertexBuffer, NumVertices, primitive_type::Triangles);
        NumVertices = 0;
      }
      Shader = Command->Shader;
      Texture = Command->Texture;
    }

    vertex Quad[6];
    MakeQuad(Quad, Command->Rect.x, Command->Rect.y,
             Command->Rect.w, Command->Rect.h, Command->Color,
             Texture.Width, Texture.Height, Command->Rotation, 
             ((Command->Flags & (u32)draw_command_flags::Flip) == (u32)draw_command_flags::Flip));
    PushVertices(VertexBuffer, &NumVertices, MAX_DRAW_COMMANDS * 6, Quad, 6);
  }

  if (NumVertices > 0) {
    if (RenderStateChanged) {
      Callbacks->PlatformUseShader(Shader);
      Callbacks->PlatformUseTexture(Texture);
    }
    Callbacks->PlatformDrawVertices(VertexBuffer, NumVertices, primitive_type::Triangles);
  }

  GameState->NumDrawCommands = 1;
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
  Callbacks->PlatformPlaySound(GameState->DeathSoundHandle);
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
      Player.Rot = 0.f;
      Player.Flags &= ~(u32)entity_flags::Flip;
    } else if (Input->IsKeyDown(key::A)) {
      Player.Dir = vec2{-1.f, 0.f};
      Player.Rot = 0.f;
      Player.Flags |= (u32)entity_flags::Flip;
    } else if (Input->IsKeyDown(key::S)) {
      Player.Dir = vec2{0.f, 1.f};
      Player.Rot = 90.f;
      Player.Flags &= ~(u32)entity_flags::Flip;
    } else if (Input->IsKeyDown(key::W)) {
      Player.Dir = vec2{0.f, -1.f};
      Player.Rot = 90.f;
      Player.Flags |= (u32)entity_flags::Flip;
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
  Callbacks->PlatformPlaySound(GameState->CrunchSoundHandle);
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
    if ((E.Flags & (u32)entity_flags::Drawable) == (u32)entity_flags::Drawable) {
      //vertex Quad[6];
      //vec2 ScaledSize = vec2{ E.Size.x * (1.f + E.Scale), E.Size.y * (1.f + E.Scale) };
      //MakeQuad(Quad, E.Pos.x - ScaledSize.x/2.f, E.Pos.y - ScaledSize.y/2.f,
      //         ScaledSize.x, ScaledSize.y, E.Color);
      //PushVertices(GameState, Quad, 6);
      draw_command Command = { };
      Command.Texture = E.Texture;
      Command.Shader = E.Shader;
      Command.Z = E.Z;
      Command.Color = E.Color;
      Command.Rotation = E.Rot;
      vec2 ScaledSize = vec2{ E.Size.x * (1.f + E.Scale), E.Size.y * (1.f + E.Scale) };
      vec2 Position = vec2{ E.Pos.x - ScaledSize.x / 2.f, E.Pos.y - ScaledSize.y / 2.f };
      if ((E.Flags & (u32)entity_flags::Flip) == (u32)entity_flags::Flip) {
        Command.Flags |= (u32)draw_command_flags::Flip;
      }
      Command.Rect = rect{ Position.x, Position.y,
                           ScaledSize.x, ScaledSize.y };
      PushDrawCommand(GameState, &Command);
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
  //Memory->PlatformCallbacks.PlatformDrawVertices(GameState->Vertices, 
  //                                               GameState->NumVertices, 
  //                                               primitive_type::Triangles);
  FlushCommandBuffer(GameState, &Memory->PlatformCallbacks);
  DrawText(GameState, &Memory->PlatformCallbacks);
}
