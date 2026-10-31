#ifndef STRINGS_H
#define STRINGS_H
#include <ctype.h> // for isspace()

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

internal char*
TrimWhitespace(char* String) {
  char* End;
  
  while (isspace((unsigned char)*String)) {
    String++;
  }

  if (*String == 0) {
    return String;
  }

  End = String + strlen(String) - 1;
  while (End > String && isspace((unsigned char)*End)) {
    End--;
  }

  *(End + 1) = '\0';

  return String;
}

internal size_t
StringLength(const char* String) {
  size_t Count = 0;
  while (*String++) {
    ++Count;
  }
  return (Count);
}

#endif
