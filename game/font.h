#ifndef FONT_H

#include "default_font.h"

struct font_glyph {
  u32 X;
  u32 Y;
  u32 Width;
  u32 Height;
  i32 XOffset;
  i32 YOffset;
  i32 XAdvance;
};

struct font_data {
  texture_id Texture;
  font_glyph* Atlas;
  u32 CharHeight;
};

internal font_data
LoadDefaultFont(Arena* PermArena, platform_callbacks* Callbacks) {
  font_data FontData;
  u32 AtlasLen = ArrayLength(DefaultAtlas);
  FontData.Atlas = arena_alloc(PermArena, sizeof(font_glyph) * AtlasLen);
  FontData.Texture = Callbacks->PlatformLoadTextureFromMemory(
                                                DefaultAtlasTextureData, 
                                                DEFAULT_ATLAS_WIDTH, 
                                                DEFAULT_ATLAS_HEIGHT);

  u32 AtlasIdx = 0;
  for (u32 I = DEFAULT_ATLAS_FONT; I < AtlasLen; ++I) {
    font_glyph Glyph = font_glyph{
      DefaultAtlas[I].x,
      DefaultAtlas[I].y,
      DefaultAtlas[I].w,
      DefaultAtlas[I].h,
      0, 0,
      DefaultAtlas[I].w
    };
    FontData.Atlas[AtlasIdx] = Glyph;
    AtlasIdx += 1;
  }
  FontData.CharHeight = DEFAULT_ATLAS_CHAR_HEIGHT;
  return FontData;
}

#define FONT_H
#endif
