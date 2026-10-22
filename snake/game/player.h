#ifndef PLAYER_H
#define PLAYER_H

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

internal void
CreatePlayer(game_state* GameState) {
  GameState->PlayerRef = GameState->Entities->Add(kind::Player);
  entity& Player = GameState->Entities->Get(GameState->PlayerRef);
  Player.Pos = PlayerSettings.StartPosition;
  Player.Size = PlayerSettings.Size;
  Player.Collider = PlayerSettings.Collider;
  Player.Color = PlayerSettings.Color;
  // TODO: snake segments instead?
  //Player.Texture = GameState->PacmanTextureHandle;
  Player.Flags |= (u32)entity_flags::Drawable;
  Log("Created player handle: %i with texture handle: %u [%f:%f]", GameState->PlayerRef.Idx, Player.Texture.Handle, Player.Texture.Width, Player.Texture.Height);
}

internal vec2
GetRandomPointAwayFromPlayer(game_state* GameState, f32 Radius) {
  vec2 PlayerPosition = vec2{WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f};
  entity& Player = GameState->Entities->Get(GameState->PlayerRef);
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

#endif
