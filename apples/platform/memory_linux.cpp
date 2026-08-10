int
AllocateContiguousMemoryBuffer(void** Ptr, size_t Size) {
  *Ptr = mmap(NULL, Size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (*Ptr == MAP_FAILED) {
    printf("Failed to reserve memory of %lu bytes.\n", Size);
    return 1;
  } else {
    int result = mprotect(*Ptr, Size, PROT_READ | PROT_WRITE);
    if (result == -1) {
      printf("Failed to commit memory of %lu bytes.\n", Size);
      return 1;
    }
  }
  return 0;
}

int
AllocatePlatformMemory(game_memory* Memory) {
  Memory->PermanentStorageSize = Megabytes(16);
  Memory->ScratchStorageSize = Megabytes(16);

  if (AllocateContiguousMemoryBuffer(&Memory->PermanentStorage, Memory->PermanentStorageSize) != 0) {
    printf("Failed to allocate permanent storage.");
    return 1;
  }
  if (AllocateContiguousMemoryBuffer(&Memory->ScratchStorage, Memory->ScratchStorageSize) != 0) {
    printf("Failed to allocate scratch storage.");
    return 1;
  }
  return 0;
}
