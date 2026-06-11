int
AllocatePlatformMemory(game_memory* Memory) {
  Memory->PermanentStorageSize = Megabytes(16);
  Memory->ScratchStorageSize = Megabytes(16);

  Memory->PermanentStorage = VirtualAlloc(0, Memory->PermanentStorageSize,
                                          MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  Memory->ScratchStorage = VirtualAlloc(0, Memory->ScratchStorageSize,
                                          MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  if ((Memory->PermanentStorage != 0) || (Memory->ScratchStorage != 0)) {
    return 1;
  }

  return 0;
}
