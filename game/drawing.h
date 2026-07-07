#ifndef DRAWING_H

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

struct command_stack {
  draw_command* Buffer;
  u32 BufferLength;
  u32 NumCommands;
};

void
CommandStackInit(command_stack* CommandStack, void* Backbuffer, u32 BufferLength) {
  CommandStack->BufferLength = BufferLength;
  CommandStack->Buffer = (draw_command*)Backbuffer;
  CommandStack->NumCommands = 1;
}

inline void
PushDrawCommand(command_stack* CommandStack, draw_command* Command) {
  draw_command* DestCommand = (CommandStack->NumCommands < CommandStack->BufferLength
                               ? &CommandStack->Buffer[CommandStack->NumCommands++]
                               : &CommandStack->Buffer[0]);
  *DestCommand = *Command;
}


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

internal int
CompareCommands(const void* A, const void* B) {
  draw_command* CmdA = (draw_command*)A;
  draw_command* CmdB = (draw_command*)B;
  u32 DrawStateA = 0, DrawStateB = 0; // 0000 ZZZZ SSSS TTTT
  DrawStateA |= (CmdA->Z << 16);
  DrawStateA |= (CmdA->Shader.Handle << 8);
  DrawStateA |= CmdA->Texture.Handle;
  DrawStateB |= (CmdB->Z << 16);
  DrawStateB |= (CmdB->Shader.Handle << 8);
  DrawStateB |= CmdB->Texture.Handle;
  return ((i32)DrawStateA - (i32)DrawStateB);
}

internal void
PushVertices(vertex* VerticesDst, u32* DstNumVertices, u32 DstMaxVertices,
             vertex* VerticesToPush, u32 NumVerticesToPush) {
  u32 NumVerticesToCopy = MIN(DstMaxVertices - *DstNumVertices, NumVerticesToPush);
  size_t BytesToCopy = (size_t)(NumVerticesToCopy * sizeof(VerticesToPush[0]));
  vertex* VerticesPtr = &VerticesDst[*DstNumVertices];
  memcpy((void*)VerticesPtr, (const void*)VerticesToPush, BytesToCopy);
  *DstNumVertices += NumVerticesToCopy;
}

internal void
FlushCommandStack(command_stack* CommandStack, platform_callbacks* Callbacks) {
  qsort(CommandStack->Buffer+1, 
        CommandStack->NumCommands-1, 
        sizeof(draw_command), 
        CompareCommands);
  texture_id Texture = { };
  shader_id Shader = { };
  vertex VertexBuffer[CommandStack->BufferLength * 6];
  u32 NumVertices = 0;
  b32 RenderStateChanged = false;
  for (u32 I = 1; I < CommandStack->NumCommands; ++I) {
    draw_command* Command = &CommandStack->Buffer[I];
    RenderStateChanged = ((Command->Shader.Handle != Shader.Handle)
                       || (Command->Texture.Handle != Texture.Handle));
    if (RenderStateChanged) {
      if (NumVertices > 0) {
        Callbacks->PlatformUseShader(Shader);
        Callbacks->PlatformUseTexture(Texture);
        Callbacks->PlatformDrawVertices(VertexBuffer, NumVertices, primitive_type::Triangles);
        NumVertices = 0;
      }
      Shader = Command->Shader;
      Texture = Command->Texture;
    }

    vertex Quad[6];
    MakeQuad(Quad, Command->Rect.x, Command->Rect.y,
             Command->Rect.w, Command->Rect.h, Command->Color,
             Texture.Width, Texture.Height, Command->Rotation, 
             (Command->Flags & (u32)draw_command_flags::Flip));
    PushVertices(VertexBuffer, &NumVertices, MAX_DRAW_COMMANDS * 6, Quad, 6);
  }

  if (NumVertices > 0) {
    if (RenderStateChanged) {
      Callbacks->PlatformUseShader(Shader);
      Callbacks->PlatformUseTexture(Texture);
    }
    Callbacks->PlatformDrawVertices(VertexBuffer, NumVertices, primitive_type::Triangles);
  }

  CommandStack->NumCommands = 1;
}

#define DRAWING_H
#endif
