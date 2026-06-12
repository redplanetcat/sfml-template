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

internal void 
MakeQuad(vertex* v, f32 x, f32 y, f32 width, f32 height, color c) {
  v[0] = {{x, y}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[1] = {{x+width, y}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[2] = {{x+width, y+height}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[3] = {{x, y}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[4] = {{x, y+height}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
  v[5] = {{x+width, y+height}, { c.r, c.g, c.b, c.a }, {0.f, 0.f}};
}

#define GRAPHICS_H
#endif
