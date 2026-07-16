#ifndef GUI_H

#include "../common/hash.h"

#define MAX_GUI_CONTAINERS 2048
#define MAX_GUI_IDS 2048

namespace gui {

struct element_id {
  u32 Id;
};

enum class layout: u32 {
  Vertical,
  Horizontal,
};

enum class container_flags: u32 {
  Hidden,
  FitContent,
};

enum class margin: u32 {
  Top,
  Right,
  Bottom,
  Left
};

struct style {
  color BaseColor;
  color HotColor;
  color ActiveColor;
  color TextColor;
  f32 ButtonMargin[4];
};

global const style DefaultStyle = {
  {100, 100, 100, 255}, // BaseColor
  {100, 200, 100, 255}, // HotColor
  {200, 100, 100, 255}, // ActiveColor
  {255, 255, 255, 255}, // TextColor
  {4.f, 8.f, 4.f, 8.f} // ButtonMargin
};

struct container {
  rect Rect;
  f32 Margin[4];
  u32 Z;
  u32 Flags;
  style Style;
  layout Layout;
};

struct context {
  /* Active elements */
  element_id Hot;
  element_id Active;

  /* ID stack */
  element_id IDs[MAX_GUI_IDS];
  u32 NumIDs;

  /* Container stack */
  container Containers[MAX_GUI_CONTAINERS];
  u32 NumContainers;

  /* Input */
  b32 LeftMousePressed;
  b32 RightMousePressed;
  b32 MiddleMousePressed;

  /* Drawing */
  style Style;
  font_data FontData;
  command_stack* CommandStack;
  u32 ZBase;
};

internal vec2
GetTextSize(const char* Text, u32 TextLen, font_data* FontData) {
  f32 Length = 0.f;
  f32 Height = (f32)FontData->CharHeight;
  for (u32 I = 0; I < TextLen; ++I) {
    u8 Byte = Text[I];
    if ((Byte & 0xc0) == 0x80) continue;
    u8 Char = MIN(Byte, 127);
    font_glyph Glyph = FontData->Atlas[u32(Char)];
    Length = Length + (f32)Glyph.XAdvance;
  }
  return (vec2{Length, Height});
}

internal void
GuiInit(context* CTX, 
        command_stack* CommandStack, 
        arena* PermArena, 
        arena* ScratchArena,
        platform_callbacks* Callbacks) 
{
  CTX->NumIDs = 1;
  CTX->NumContainers = 1;
  CTX->ZBase = UINT8_MAX / 2;
  CTX->CommandStack = CommandStack;
  CTX->FontData = LoadDefaultFont(PermArena, ScratchArena, Callbacks);
  CTX->Style = DefaultStyle;
}


internal void
GuiUpdateInputState(context* CTX, game_input* Input) {
  if (!Input->IsMouseButtonDown(mouse_button::Left)) {
    CTX->Active = {};
  }
}

internal void
GuiUpdateElementInput(context* CTX, element_id Id) {

}

internal element_id
GuiGetId(context* CTX, const void* Data, u32 Size) {
  element_id Id = {};
  Id.Id = (CTX->NumIDs > 0) ? CTX->IDs[CTX->NumIDs - 1].Id : HASH_INITIAL;
  Hash(&Id.Id, Data, Size);
  return (Id);
}

internal void
GuiPushId(context* CTX, const void* Data, u32 Size) {
  element_id Id = GuiGetId(CTX, Data, Size);
  Assert(CTX->NumIDs < (sizeof(CTX->IDs) / sizeof(element_id)));
  CTX->IDs[CTX->NumIDs] = Id;
  CTX->NumIDs++;
}

internal void
GuiPopId(context* CTX) {
  Assert(CTX->NumIDs > 1);
  CTX->NumIDs--;
}

internal void
GuiPushContainer(context* CTX, container* Container) {
  Assert(CTX->NumContainers < (sizeof(CTX->Containers) / sizeof(container)));
  CTX->Containers[CTX->NumContainers] = *Container;
  CTX->NumContainers++;
}

internal void
GuiPopContainer(context* CTX) {
  Assert(CTX->NumContainers > 1);
  CTX->NumContainers--;
}

internal container
CreateChildContainer(context* CTX, container* Parent) {
  container Container = {};
  Container.Rect = {
    Parent->Rect.x + Parent->Margin[(u32)margin::Left],
    Parent->Rect.y + Parent->Margin[(u32)margin::Top],
    Parent->Rect.w - 
      (Parent->Margin[(u32)margin::Right] + Parent->Margin[(u32)margin::Left]),
    Parent->Rect.h - 
      (Parent->Margin[(u32)margin::Top] + Parent->Margin[(u32)margin::Bottom])
  };
  memset(Container.Margin, 0, sizeof(Container.Margin));
  Container.Z = Parent->Z + 1;
  Container.Flags = Parent->Flags;
  return (Container);
}

internal void
GuiDrawPanel(context* CTX, container* Container) {
  draw_command Command = { };
  //Command.Texture = E.Texture;
  //Command.Shader = E.Shader;
  //Command.Z = E.Z;
  //Command.Color = E.Color;
  //Command.Rotation = E.Rot;
  //vec2 ScaledSize = vec2{ E.Size.x * (1.f + E.Scale), E.Size.y * (1.f + E.Scale) };
  //vec2 Position = vec2{ E.Pos.x - ScaledSize.x / 2.f, E.Pos.y - ScaledSize.y / 2.f };
  //if (E.Flags & (u32)entity_flags::Flip) {
  //  Command.Flags |= (u32)draw_command_flags::Flip;
  //}
  Command.Color = Container->Style.BaseColor;
  Command.DstRect = Container->Rect;
  Command.Z = Container->Z;
  PushDrawCommand(CTX->CommandStack, &Command);
}

internal void
GuiDrawButton(context* CTX, container* Container) {
  draw_command Command = { };
  Command.Color = Container->Style.BaseColor;
  Command.DstRect = Container->Rect;
  Command.Z = Container->Z;
  PushDrawCommand(CTX->CommandStack, &Command);
}

internal void
GuiBegin(context* CTX) {
  CTX->NumIDs = 1;
  CTX->NumContainers = 1;

  container Container = {};
  Container.Rect = {
    0, 0,
    WINDOW_WIDTH, WINDOW_HEIGHT
  };
  memset(Container.Margin, 0, sizeof(Container.Margin));
  Container.Z = CTX->ZBase;
  Container.Flags = 0;
  Container.Style = CTX->Style;
  Container.Layout = layout::Vertical;
  GuiPushContainer(CTX, &Container);

}

internal void
GuiEnd(context* CTX) {
  
}

internal void
PanelBegin(context* CTX, const char* Name,
           layout Layout = layout::Vertical) 
{
  GuiPushId(CTX, (const void*) Name, (u32)strlen(Name));
  container Container = CreateChildContainer(CTX, 
                                    &CTX->Containers[CTX->NumContainers - 1]);
  Container.Style = CTX->Style;
  Container.Layout = Layout;
  GuiPushContainer(CTX, &Container);
  GuiDrawPanel(CTX, &Container);
}

internal void
PanelEnd(context* CTX) {
  GuiPopContainer(CTX);
  GuiPopId(CTX);
}

internal void
Label(context* CTX, const char* Text) {
  u32 TextLen = (u32)strlen(Text);
  container Parent = CTX->Containers[CTX->NumContainers-1];
  vec2 Pos = vec2{ Parent.Rect.x + Parent.Margin[(u32)margin::Left], 
                   Parent.Rect.y + Parent.Margin[(u32)margin::Top ] };
  color Color = CTX->Style.TextColor;
  DrawText(Text,
           TextLen,
           &CTX->FontData, 
           Pos, 
           Color, 
           Parent.Z + 1,
           CTX->CommandStack); 
  
}

internal void
Image(context* CTX, image* Image) {

}

internal b32
Button(context* CTX, const char* Text) {
  b32 Clicked = false;
  u32 TextLen = (u32)strlen(Text);
  element_id Id = GuiGetId(CTX, (const void*)Text, TextLen);
  vec2 TextSize = GetTextSize(Text, TextLen, &CTX->FontData);
  container Container = CreateChildContainer(CTX, 
                                    &CTX->Containers[CTX->NumContainers - 1]);
  Container.Style = CTX->Style;
  Container.Layout = layout::Horizontal;
  memcpy(Container.Margin, CTX->Style.ButtonMargin, sizeof(Container.Margin));
  Container.Rect.w = TextSize.x + 
    Container.Margin[(u32)margin::Left] + Container.Margin[(u32)margin::Right];
  Container.Rect.h = TextSize.y +
    Container.Margin[(u32)margin::Top] + Container.Margin[(u32)margin::Bottom];
  GuiPushContainer(CTX, &Container);
  Label(CTX, Text);
  GuiPopContainer(CTX);

  GuiUpdateElementInput(CTX, Id);

  if (CTX->Active.Id == Id.Id) {
    Clicked = true;
  }

  GuiDrawButton(CTX, &Container);
  return (Clicked);
}

internal b32
ImageButton(context* CTX, image* Image) {
  return false;
}

} // namespace gui

#define GUI_H
#endif
