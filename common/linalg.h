#ifndef LINALG_H

struct irect {
  i32 x;
  i32 y;
  i32 w;
  i32 h;
};

struct rect {
  f32 x;
  f32 y;
  f32 w;
  f32 h;
};

struct ivec2 {
  i32 x;
  i32 y;
};

struct vec2 {
  f32 x;
  f32 y;
  
  internal inline f32 DistanceToLineSq(const vec2& Start, const vec2& End, const vec2& Point) {
    const f32 A = End.y - Start.y;
    const f32 B = Start.x - End.x;
    const f32 C = Start.y * (End.x - Start.x) - Start.x * (End.y - Start.y);
    const f32 Top = (A * Point.x + B * Point.y + C);
    const f32 Bottom = A * A + B * B;
    return ((Top * Top) / Bottom);
  }

  b32 IsInRect(const rect& Rect) {
    if (
        (x <= Rect.x + Rect.w)
        & (y <= Rect.y + Rect.h)
        & (x >= Rect.x)
        & (y >= Rect.y)
    ) {
      return true;
    }
    return false;
  }
  
  inline f32 LenSq() const {
    return (x * x + y * y);
  }

  inline f32 DistanceToSq(const vec2 V) const {
    return ((V.x - x) * (V.x - x) + (V.y - y) * (V.y - y));
  }

  vec2 operator+(const vec2 V) const {
    return (vec2{x + V.x, y + V.y});
  }

  vec2& operator+=(const vec2 V) {
    x += V.x;
    y += V.y;
    return (*this);
  }

  vec2 operator-(const vec2 V) const {
    return (vec2{x - V.x, y - V.y});
  }

  vec2& operator-=(const vec2 V) {
    x -= V.x;
    y -= V.y;
    return (*this);
  }

  vec2 operator*(const f32 A) const {
    return (vec2{x * A, y * A});
  }

  vec2& operator*=(const f32 A) {
    x *= A;
    y *= A;
    return (*this);
  }

  b32 operator==(const vec2 V) const {
    return (x == V.x && y == V.y);
  }

  f32 AngleTo(const vec2& V) const {
    const f32 dot = x * V.x + y * V.y;
    const f32 det = x * V.y - y * V.x;
    return (f32(atan2(det, dot)));
  }

  vec2& Rotate(const f32 AngleRadians) {
    f32 C = f32(cos(AngleRadians));
    f32 S = f32(sin(AngleRadians));
    f32 px = x * C - y * S;
    f32 py = x * S + y * C;
    x = px;
    y = py;
    return (*this);
  }

  vec2& Normalize() {
    const f32 Length = sqrtf(LenSq());
    x = x / Length;
    y = y / Length;
    return (*this);
  }

  f32 NormalizeAndGetLength() {
    const f32 Length = sqrtf(LenSq());
    x = x / Length;
    y = y / Length;
    return Length;
  }
};

#define LINALG_H
#endif
