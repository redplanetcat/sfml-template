#ifndef HASH_H

#define HASH_INITIAL 2166136261

/* 32-bit fnv-1a */
internal void
CalculateHash(u32* Hash, const void* Data, u32 Size) {
  const u8* P = (const u8*)Data;
  while (Size--) {
    *Hash = (*Hash ^ *P++) * 16777619;
  }
}

#define HASH_H
#endif
