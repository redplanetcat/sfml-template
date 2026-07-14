#ifndef FONT_H

struct font_glyph {
  i32 X;
  i32 Y;
  i32 Width;
  i32 Height;
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
LoadDefaultFont(arena* PermArena, platform_callbacks* Callbacks) {
  font_data FontData;
  u32 AtlasLen = ArrayCount(DefaultAtlas);
  FontData.Atlas = (font_glyph*)ArenaAlloc(PermArena, sizeof(font_glyph) * AtlasLen);
  FontData.Texture = Callbacks->PlatformLoadTextureFromMemory(
                                                (void*)DefaultAtlasTextureData, 
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
