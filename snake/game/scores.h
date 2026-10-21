#ifndef SCORES_H

COMPARE_FUNCTION(ScoreCompare) {
  hashmap_node* NodeA = *(hashmap_node**)A;
  hashmap_node* NodeB = *(hashmap_node**)B;
  u32 ValueA = *(u32*)NodeA->Value;
  u32 ValueB = *(u32*)NodeB->Value;
  return ((i32)ValueB - (i32)ValueA);
}

internal void
SortLeaderboard(hashmap* HighScores) {
  HashmapSort(HighScores, QuickSort, ScoreCompare);
}

internal void
LoadLeaderboardFromFile(hashmap* HighScores, 
                        const char* FilePath, 
                        platform_callbacks* Callbacks) {
  u32 FileSize = Callbacks->PlatformGetFileLength(FilePath);
  if (FileSize == 0) {
    return;
  }

  char* FileBuffer = (char*)ArenaAlloc(&ScratchArena, FileSize * sizeof(char));
  Callbacks->PlatformReadEntireFile(FileBuffer, FilePath, FileSize);

  char LineCopy[512];
  memset(LineCopy, 0, 512);
  char* NextLine = strpbrk(FileBuffer, "\n\0;");
  memcpy(LineCopy, FileBuffer, (uintptr_t)(NextLine - FileBuffer));

  while (NextLine != NULL) {
    size_t LineLen = strnlen(LineCopy, 512);

    if (LineCopy[LineLen-1] == '\r') {
      LineCopy[LineLen-1] = '\0';
      LineLen--;
    }

    char* Rest = LineCopy;
    strtok_r(LineCopy, ":", &Rest);

    size_t NameLen = strnlen(LineCopy, 512);
    u32 Value = (u32)strtoul(Rest, NULL, 10);

    HashmapInsert(HighScores, (const void*)LineCopy, (u32)NameLen, (const void*)&Value);

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

  SortLeaderboard(HighScores);
}

internal void
SaveLeaderboardToFile(hashmap* HighScores,
                      const char* FilePath,
                      platform_callbacks* Callbacks) {
  u32 BufferLength = HashmapGetElementCount(HighScores) * 128;
  char* Buffer = (char*)ArenaAlloc(&ScratchArena, BufferLength);
  memset(Buffer, 0, BufferLength);

  struct hashmap_node* ScoreNode = NULL;
  u32 ScoreIndex = 0;
  u32 Length = 0;
  while (HashmapIterNodes(HighScores, &ScoreIndex, &ScoreNode)) {
    char* ScoreName = &HighScores->Keys[HighScores->KeySize * ScoreNode->KeyIdx];
    Length += (u32)snprintf(Buffer + Length, BufferLength, "%s: %u\n", ScoreName, *((u32*)ScoreNode->Value));
  }
  printf("Writing to file:\n%s\n", Buffer);

  Callbacks->PlatformWriteToFile(Buffer, FilePath, Length + 1);
}

#define SCORES_H
#endif
