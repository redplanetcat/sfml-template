internal void
CatStrings(size_t SourceACount, const char* SourceA,
           size_t SourceBCount, const char* SourceB,
           size_t DestCount, char* Dest) 
{
  for (size_t Index = 0; Index < SourceACount; ++Index) {
    *Dest++ = *SourceA++;
  }

  for (size_t Index = 0; Index < SourceBCount; ++Index) {
    *Dest++ = *SourceB++;
  }

  *Dest++ = 0;
}

internal size_t
StringLength(const char* String) {
  size_t Count = 0;
  while (*String++) {
    ++Count;
  }
  return (Count);
}
