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


internal void
OnPlayerDied(game_state* GameState, platform_callbacks* Callbacks) {
  entity_manager& EM = *GameState->Entities;
  entity& Player = EM.Get(GameState->PlayerRef);
  if (Player) {
    EM.Rem(GameState->PlayerRef);
  }
  GameState->RestartTimer = PlayerSettings.RestartTime;
  Callbacks->PlatformPlaySound(GameState->DeathSoundHandle);
}

internal void
UpdatePlayer(game_state* GameState, 
             platform_callbacks* Callbacks, 
             game_input* Input, 
             f32 Delta) 
{
  entity& Player = GameState->Entities->Get(GameState->PlayerRef);
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
      Log("Out of bounds death!");
      OnPlayerDied(GameState, Callbacks);
    }
  } else if (GameState->RestartTimer <= 0.f) {
    GameReset(GameState, Callbacks);
  }

}

internal u32
CountApples(game_state* GameState) {
  entity_manager& EM = *GameState->Entities;
  u32 Count = 0;
  for (auto EIter = EM.begin(); EIter != EM.end(); ++EIter) {
    if (EIter->Kind == kind::Apple) {
      Count += 1;
    }
  }
  return Count;
}

internal void
OnAppleCollect(entity_ref AppleRef, game_state* GameState, platform_callbacks* Callbacks) {
  entity_manager& EM = *GameState->Entities;
  EM.Rem(AppleRef);
  UpdateScore(GameState, Callbacks, GameState->Score + PlayerSettings.AppleScore);
  if (GameState->GameMode & (1 << (u32)game_mode::SpeedUpOnScore)) {
    GameState->PlayerSpeed += PlayerSettings.SpeedDelta;
  }
  if (GameState->GameMode & (1 << (u32)game_mode::InfiniteApples)) {
    SpawnApple(GameState);
  }
  // TODO: add a segment to snake tail
  if (GameState->Score % 1000 == 0) {
    SpawnStone(GameState);
  }
  Callbacks->PlatformPlaySound(GameState->CrunchSoundHandle);
  u32 ApplesCount = CountApples(GameState);
  if (ApplesCount <= 0) {
    GameState->Scene = game_scene::WinScreen;
  }
}

internal void
OnPlayerPickup(entity_ref PickupRef, game_state* GameState, platform_callbacks* Callbacks) {
  entity_manager& EM = *GameState->Entities;
  entity& EPickup = EM.Get(PickupRef);
  if (EPickup.Kind == kind::Apple) {
    OnAppleCollect(PickupRef, GameState, Callbacks);
  }
}

#endif
