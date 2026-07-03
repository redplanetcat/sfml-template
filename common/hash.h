#ifndef HASH_H

#define HASH_INITIAL 2166136261

/* 32-bit fnv-1a */
internal void
Hash(u32* Hash, const void* Data, u32 Size) {
  const u8* p = data;
  while (Size--) {
    *Hash = (*Hash ^ *p++) * 16777619;
  }
}

#define HASH_H
#endif
