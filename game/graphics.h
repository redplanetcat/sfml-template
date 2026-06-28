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

enum class draw_command_flags : u32 {
  Flip = 1 << 0,
};

struct draw_command {
  texture_id Texture;
  shader_id Shader;
  u32 Z;
  color Color;
  f32 Rotation;
  rect Rect;
  u32 Flags;
};


#define GRAPHICS_H
#endif
