#ifndef RESOURCES_H
#define RESOURCES_H

internal b32
LoadResources(game_state* GameState, const char* FilePath, platform_callbacks* Callbacks) {
  u32 FileSize = Callbacks->PlatformGetFileLength(FilePath);
  if (FileSize == 0) {
    return false;
  }

  char* FileBuffer = (char*)ArenaAlloc(&ScratchArena, FileSize * sizeof(char));
  Callbacks->PlatformReadEntireFile(FileBuffer, FilePath, FileSize);
  
  char LineCopy[512];
  memset(LineCopy, 0, 512);
  char* NextLine = strpbrk(FileBuffer, "\n\0;");
  memcpy(LineCopy, FileBuffer, (uintptr_t(NextLine - FileBuffer)));
  
  while (NextLine != NULL) {
    size_t LineLen = strnlen(LineCopy, 512);
    
    if (LineCopy[LineLen-1] == '\r') {
      LineCopy[LineLen-1] = '\0';
      LineLen--;
    }

    char* Rest = LineCopy;
    strtok_r(LineCopy, ":", &Rest);

    size_t NameLen = strnlen(LineCopy, 512);
    size_t PathLen = strnlen(Rest, uintptr_t(LineCopy - Rest));

    for (i32 I = 0; I < (i32)snake_sprite::Count; I++) {
      if (GameState->SnakeTextureHandles[I].Handle > 0) {
        // if texture was already loaded, skip
        continue;
      }
      //compare name, load texture
      if (strncmp(LineCopy, SnakeString[I], NameLen) == 0) {
        GameState->SnakeTextureHandles[I] = Callbacks->PlatformLoadTexture(Rest);
        Log("Snake %s texture handle: %u", SnakeString[I], GameState->SnakeTextureHandles[I]);
      }
    }

    char* OldLine = NextLine + 1;
    NextLine = strpbrk(NextLine + 1, "\n\0;");

    if (NextLine >= FileBuffer + FileSize) {
      break;
    }

    if (NextLine != NULL) {
      memset(LineCopy, 0, 512);
      memcpy(LineCopy, OldLine, (uintptr_t)(NextLine - OldLine));
    }
  }

  if (GameState->AppleTextureHandle.Handle == 0) {
    GameState->AppleTextureHandle = Callbacks->PlatformLoadTexture("Resources/Textures/Apple.png");
    Log("Apple texture handle: %u", GameState->AppleTextureHandle.Handle);
  }
  if (GameState->RockTextureHandle.Handle == 0) {
      GameState->RockTextureHandle = Callbacks->PlatformLoadTexture("Resources/Textures/Rock.png");
      Log("Rock texture handle: %u", GameState->RockTextureHandle.Handle);
  }
  if (GameState->CrunchSoundHandle.Handle == 0) {
    GameState->CrunchSoundHandle = Callbacks->PlatformLoadSound("Resources/Sounds/Crunch.ogg");
    Log("Crunch sound handle: %u", GameState->CrunchSoundHandle.Handle);
  }
  if (GameState->DeathSoundHandle.Handle == 0) {
    GameState->DeathSoundHandle = Callbacks->PlatformLoadSound("Resources/Sounds/Death.ogg");
    Log("Death sound handle: %u", GameState->DeathSoundHandle.Handle);
  }

  return true;
}

#endif
