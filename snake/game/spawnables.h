#ifndef SPAWNABLES_H
#define SPAWNABLES_H

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

internal void
SpawnApple(game_state* GameState) {
  vec2 ApplePosition = GetRandomPointAwayFromPlayer(GameState, 64);
  entity_ref AppleRef = GameState->Entities->Add(kind::Apple);
  entity& Apple = GameState->Entities->Get(AppleRef);
  Apple.Pos = ApplePosition;
  Apple.Size = vec2{16.f, 16.f};
  Apple.Collider = Apple.Size;
  Apple.Color = color{255, 255, 255, 255};
  Apple.Texture = GameState->AppleTextureHandle;
  Apple.Timer = 0.5f;
  Apple.TimerLength = 0.5f;
  Apple.Flags |= (u32)entity_flags::Drawable;
  Apple.Flags |= (u32)entity_flags::Pickup;
  Apple.Flags |= (u32)entity_flags::SpawnAnimated;
  Log("Created apple handle: %i with texture handle: %u [%f:%f]", AppleRef.Idx, Apple.Texture.Handle, Apple.Texture.Width, Apple.Texture.Height);
}

internal void
SpawnStartingApples(game_state* GameState) {
  // TODO: spawn apples one by one instead
  //u32 MaxApplesToSpawn = (GameState->GameMode & (1 << (u32)game_mode::ManyApples))
  //            ? PlayerSettings.NumStartingApplesB
  //            : PlayerSettings.NumStartingApplesA;
  //u32 MinApplesToSpawn = MaxApplesToSpawn / 2;
  //u32 ApplesToSpawn = (u32)(((f32)rand() / (f32)RAND_MAX) *
  //            ((f32)MaxApplesToSpawn-(f32)MinApplesToSpawn) + (f32)MinApplesToSpawn);
  //for (u32 I = 0; I <= ApplesToSpawn; I++) {
  //  SpawnApple(GameState);
  //}
}

internal void
SpawnStone(game_state* GameState) {
  vec2 StonePosition = GetRandomPointAwayFromPlayer(GameState, 64);
  entity_ref StoneRef = GameState->Entities->Add(kind::Stone);
  Log("Created stone handle: %i", StoneRef.Idx);
  entity& Stone = GameState->Entities->Get(StoneRef);
  Stone.Pos = StonePosition;
  Stone.Size = vec2{32.f, 32.f};
  Stone.Collider = Stone.Size;
  Stone.Color = color{100, 100, 100, 255};
  Stone.Texture = GameState->RockTextureHandle;
  Stone.Timer = 0.5f;
  Stone.TimerLength = 0.5f;
  Stone.Flags |= (u32)entity_flags::Drawable;
  Stone.Flags |= (u32)entity_flags::Damager;
  Stone.Flags |= (u32)entity_flags::SpawnAnimated;
}

#endif
