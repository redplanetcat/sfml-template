#ifndef SNAKE_H
#define SNAKE_H

#define SNAKE_TABLE \
  X(HeadUp) \
  X(HeadRight) \
  X(HeadDown) \
  X(HeadLeft) \
  X(BodyVertical) \
  X(BodyHorizontal) \
  X(BodyBottomRight) \
  X(BodyBottomLeft) \
  X(BodyTopRight) \
  X(BodyTopLeft) \
  X(TailUp) \
  X(TailRight) \
  X(TailDown) \
  X(TailLeft)

#define X(name) name,
enum class snake_sprite: u32 {
  SNAKE_TABLE
  Count
};
#undef X

#define X(name) #name,
static const char* const SnakeString[] = {
  SNAKE_TABLE
};
#undef X


#endif
