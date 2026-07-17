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
  Expand,
};

enum class margin: u32 {
  Top,
  Right,
  Bottom,
  Left
};

enum class align: u8 {
  Top,
  Center,
  Bottom
};

enum class justify: u8 {
  Left,
  Center,
  Right
};

struct style {
  color BaseColor;
  color HotColor;
  color ActiveColor;
  color TextColor;
  color ShadowColor;
  f32 ButtonMargin[4];
  f32 HorizontalGap;
  f32 VerticalGap;
};

global const style DefaultStyle = {
  {100, 100, 100, 255}, // BaseColor
  {100, 200, 100, 255}, // HotColor
  {200, 100, 100, 255}, // ActiveColor
  {255, 255, 255, 255}, // TextColor
  { 50,  50,  50, 255}, // ShadowColor
  {4.f, 8.f, 4.f, 8.f}, // ButtonMargin
  8.f, // Horizontal Gap
  16.f, // Vertical Gap
};

struct container {
  rect Rect;
  f32 Margin[4];
  f32 Content[2];
  f32 Gap;
  u32 Z;
  u32 Flags;
  style Style;
  layout Layout;
  align Align;
  justify Justify;
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

internal container*
GuiPopContainer(context* CTX) {
  Assert(CTX->NumContainers > 1);
  container* Container = &CTX->Containers[CTX->NumContainers - 1];
  CTX->NumContainers -= 1;
  return (Container);
}

internal container
CreateChildContainer(context* CTX, 
                     container* Parent, 
                     f32 Width = 0.f,
                     f32 Height = 0.f,
                     layout Layout = layout::Horizontal) 
{
  container Container = {};
  f32 LeftMargin = Parent->Margin[(u32)margin::Left];
  f32 TopMargin = Parent->Margin[(u32)margin::Top];
  f32 RightMargin = Parent->Margin[(u32)margin::Right];
  f32 BottomMargin = Parent->Margin[(u32)margin::Bottom];
  f32 LeftContent = Parent->Content[(u32)layout::Horizontal];
  f32 TopContent = Parent->Content[(u32)layout::Vertical];
  f32 LeftGap = (LeftContent > 0.f ? Parent->Gap : 0.f);
  f32 TopGap = (TopContent > 0.f ? Parent->Gap : 0.f);
  Container.Rect.x = Parent->Rect.x + LeftMargin + (Parent->Layout == layout::Horizontal
        ? LeftContent + LeftGap
        : 0.f);
  Container.Rect.y = Parent->Rect.y + TopMargin + (Parent->Layout == layout::Vertical
        ? TopContent + TopGap
        : 0.f);
  Container.Rect.w = Width;
  Container.Rect.h = Height;
  Parent->Content[(u32)layout::Horizontal] = (Parent->Layout == layout::Horizontal
        ? LeftContent + LeftGap + Container.Rect.w
        : Container.Rect.w);
  Parent->Content[(u32)layout::Vertical] = (Parent->Layout == layout::Vertical
        ? TopContent + TopGap + Container.Rect.h
        : Container.Rect.h);
  Container.Z = Parent->Z + 1;
  Container.Flags = Parent->Flags;
  Container.Gap = (Layout == layout::Horizontal 
                   ? CTX->Style.HorizontalGap
                   : CTX->Style.VerticalGap);
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
  Command.Color.a = 150;
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
}

internal void
PanelEnd(context* CTX) {
  container* Panel = GuiPopContainer(CTX);
  container* Parent = &CTX->Containers[CTX->NumContainers-1];
  Panel->Rect.w = Panel->Content[(u32)layout::Horizontal] 
                  + Panel->Margin[(u32)margin::Left]
                  + Panel->Margin[(u32)margin::Right];
  Panel->Rect.h = Panel->Content[(u32)layout::Vertical]
                  + Panel->Margin[(u32)margin::Top]
                  + Panel->Margin[(u32)margin::Bottom];
  Parent->Content[(u32)layout::Horizontal] = Parent->Content[(u32)layout::Horizontal] 
        + Panel->Content[(u32)layout::Horizontal];
        //- (Parent->Layout == layout::Horizontal ? Parent->Gap : 0.f);
  Parent->Content[(u32)layout::Vertical] = Parent->Content[(u32)layout::Vertical] 
        + Panel->Content[(u32)layout::Vertical];
        //- (Parent->Layout == layout::Vertical ? Parent->Gap : 0.f);
  //Parent->Content[(u32)layout::Horizontal] = (Parent->Layout == layout::Horizontal
  //      ? Parent->Content[(u32)layout::Horizontal] + Parent->Gap + Panel->Rect.w
  //      : Panel->Content[(u32)layout::Horizontal]);
  //Parent->Content[(u32)layout::Vertical] = (Parent->Layout == layout::Vertical
  //      ? Panel->Content[(u32)layout::Vertical] + Parent->Gap
  //      : Panel->Content[(u32)layout::Vertical]);
  GuiDrawPanel(CTX, Panel);
  GuiPopId(CTX);
}

internal void
Label(context* CTX, const char* Text) {
  u32 TextLen = (u32)strlen(Text);
  container* Parent = &CTX->Containers[CTX->NumContainers-1];
  f32 LeftMargin = Parent->Margin[(u32)margin::Left];
  f32 TopMargin = Parent->Margin[(u32)margin::Top];
  f32 LeftContent = Parent->Content[(u32)layout::Horizontal];
  f32 TopContent = Parent->Content[(u32)layout::Vertical];
  vec2 Pos = vec2{ Parent->Rect.x + LeftMargin + LeftContent, 
                   Parent->Rect.y + TopMargin + TopContent };
  color Color = CTX->Style.TextColor;
  Parent->Content[(u32)layout::Vertical] += (CTX->FontData.CharHeight + Parent->Gap);
  DrawText(Text,
           TextLen,
           &CTX->FontData, 
           Pos, 
           Color, 
           Parent->Z + 1,
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
  f32 ButtonWidth = TextSize.x  + CTX->Style.ButtonMargin[(u32)margin::Left]
                                + CTX->Style.ButtonMargin[(u32)margin::Right];
  f32 ButtonHeight = TextSize.y + CTX->Style.ButtonMargin[(u32)margin::Top]
                                + CTX->Style.ButtonMargin[(u32)margin::Bottom];
  container Container = CreateChildContainer(CTX, 
                                    &CTX->Containers[CTX->NumContainers - 1],
                                    ButtonWidth,
                                    ButtonHeight,
                                    layout::Horizontal);
  Container.Style = CTX->Style;
  Container.Layout = layout::Horizontal;
  memcpy(Container.Margin, CTX->Style.ButtonMargin, sizeof(Container.Margin));
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
