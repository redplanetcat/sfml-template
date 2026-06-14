#ifndef GRAPHICS_H

#define MAX_VERTICES 16384

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


#define GRAPHICS_H
#endif
