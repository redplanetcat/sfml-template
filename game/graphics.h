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

internal vec2
RotatePoint(vec2 Position, vec2 Center, f32 Angle) {
  float TranslatedX = Position.x - Center.x;
  float TranslatedY = Position.y - Center.y;

  float RotatedX = TranslatedX * (f32)cos(Angle) - TranslatedY * (f32)sin(Angle);
  float RotatedY = TranslatedX * (f32)sin(Angle) + TranslatedY * (f32)cos(Angle);

  Position.x = RotatedX + Center.x;
  Position.y = RotatedY + Center.y;

  return Position;
}

internal void
RotateQuad(vertex* V, vec2 Center, f32 Rotation) {
  float RotationRad = Rotation * PI32 / 180.f;
  for (i32 I = 0; I < 6; ++I) {
    vertex* Vert = &V[I];
    Vert->Position = RotatePoint(Vert->Position, Center, RotationRad);
  }
}

internal void 
MakeQuad(vertex* V, f32 X, f32 Y, f32 Width, f32 Height, color C,
         f32 TexWidth, f32 TexHeight, f32 Rotation, b32 Flip) 
{
  f32 FlipWidth = (Flip ? TexWidth : 0.f);
  V[0] = {{X, Y}, { C.r, C.g, C.b, C.a }, {0.f + FlipWidth, 0.f}};
  V[1] = {{X+Width, Y}, { C.r, C.g, C.b, C.a }, {TexWidth - FlipWidth, 0.f}};
  V[2] = {{X+Width, Y+Height}, { C.r, C.g, C.b, C.a }, {TexWidth - FlipWidth, TexHeight}};
  V[3] = {{X, Y}, { C.r, C.g, C.b, C.a }, {0.f + FlipWidth, 0.f}};
  V[4] = {{X, Y+Height}, { C.r, C.g, C.b, C.a }, {0.f + FlipWidth, TexHeight}};
  V[5] = {{X+Width, Y+Height}, { C.r, C.g, C.b, C.a }, {TexWidth - FlipWidth, TexHeight}};

  RotateQuad(V, vec2{X + Width/2.f, Y + Height/2.f}, Rotation);

}

#define GRAPHICS_H
#endif
