#ifndef GRAPHICS_H

#define MAX_DRAW_COMMANDS 2048

struct color {
  u8 r;
  u8 g;
  u8 b;
  u8 a;
};

struct vertex {
  vec2 Position;
  color Color;
  vec2 TexCoord;
};

enum class primitive_type {
  Points,
  Lines,
  Triangles
};

struct texture_id {
  u32 Handle;
  f32 Width;
  f32 Height;
};

struct shader_id {
  u32 Handle;
};

struct sound_id {
  u32 Handle;
};

struct text_id {
  u32 Handle;
};

struct image {
  texture_id Texture;
  rect Rect;
};


#define GRAPHICS_H
#endif
