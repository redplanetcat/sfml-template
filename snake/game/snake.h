#ifndef SNAKE_H
#define SNAKE_H

enum class snake_sprite: u32 {
  HeadUp;
  HeadRight;
  HeadDown;
  HeadLeft;
  BodyVertical;
  BodyHorizontal;
  BodyBottomRight;
  BodyBottomLeft;
  BodyTopRight;
  BodyTopLeft;
  TailUp;
  TailRight;
  TailDown;
  TailLeft;
  Count;
};

#endif
