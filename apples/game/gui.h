#ifndef GUI_H

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
  vec2 Content;
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
  b32 LeftMouseDown;
  b32 RightMouseDown;
  b32 MiddleMouseDown;
  vec2 MousePosition;
  vec2 MouseDelta;
  f32 MouseWheel;

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
    u8 Byte = (u8)Text[I];
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
        arena* Arena, 
        arena* TempArena,
        platform_callbacks* Callbacks) 
{
  CTX->NumIDs = 1;
  CTX->NumContainers = 1;
  CTX->ZBase = UINT8_MAX / 2;
  CTX->CommandStack = CommandStack;
  CTX->FontData = LoadDefaultFont(Arena, TempArena, Callbacks);
  CTX->Style = DefaultStyle;
}


internal void
GuiUpdateInputState(context* CTX, game_input* Input) {
  if (!Input->IsMouseButtonDown(mouse_button::Left)) {
    CTX->Active = {};
  }
  CTX->LeftMouseDown = Input->IsMouseButtonDown(mouse_button::Left);
  CTX->RightMouseDown = Input->IsMouseButtonDown(mouse_button::Right);
  CTX->MiddleMouseDown = Input->IsMouseButtonDown(mouse_button::Middle);
  CTX->LeftMousePressed = Input->IsMouseButtonPressed(mouse_button::Left);
  CTX->RightMousePressed = Input->IsMouseButtonPressed(mouse_button::Right);
  CTX->MiddleMousePressed = Input->IsMouseButtonPressed(mouse_button::Middle);
  CTX->MousePosition = Input->MousePosition;
  CTX->MouseDelta = Input->MouseDelta;
  CTX->MouseWheel = Input->MouseWheel;
}

internal void
GuiUpdateElementInput(context* CTX, element_id Id, container* Container) {
  bool MouseOver = Container->Rect.Contains(CTX->MousePosition);
  if (MouseOver && !CTX->LeftMousePressed) {
    CTX->Active = Id;
  }

  if (CTX->Hot.Id == Id.Id) {
    if (CTX->LeftMousePressed && !MouseOver) {
      // TODO: Clear ui focus
    }
  }

  if (CTX->Active.Id == Id.Id) {
    if (CTX->LeftMousePressed) {
      // TODO: Set ui focus
    } else if (!MouseOver) {
      CTX->Active = element_id{};
    }
  }
}

internal element_id
GuiGetId(context* CTX, const void* Data, u32 Size) {
  element_id Id = {};
  Id.Id = (CTX->NumIDs > 0) ? CTX->IDs[CTX->NumIDs - 1].Id : HASH_INITIAL;
  CalculateHash(&Id.Id, Data, Size);
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
  f32 LeftContent = Parent->Content.x;
  f32 TopContent = Parent->Content.y;
  f32 LeftGap = (LeftContent > 0.f ? Parent->Gap : 0.f);
  f32 TopGap = (TopContent > 0.f ? Parent->Gap : 0.f);
  f32 LeftOffset = (Parent->Layout == layout::Horizontal) 
                                ? LeftContent + LeftGap 
                                : 0.f;
  f32 TopOffset = (Parent->Layout == layout::Vertical) 
                                ? TopContent + TopGap 
                                : 0.f;

  Container.Rect.x = Parent->Rect.x + LeftMargin + LeftOffset;
  Container.Rect.y = Parent->Rect.y + TopMargin + TopOffset;
  Container.Rect.w = Width;
  Container.Rect.h = Height;
  Parent->Content.x = (Parent->Layout == layout::Horizontal
        ? LeftContent + LeftGap + Container.Rect.w
        : MAX(Parent->Content.x, Container.Rect.w));
  Parent->Content.y = (Parent->Layout == layout::Vertical
        ? TopContent + TopGap + Container.Rect.h
        : MAX(Parent->Content.y, Container.Rect.h));
  Container.Z = Parent->Z + 1;
  Container.Gap = (Layout == layout::Horizontal 
                   ? CTX->Style.HorizontalGap
                   : CTX->Style.VerticalGap);
  return (Container);
}

internal void
GuiDrawPanel(context* CTX, container* Container) {
  draw_command Command = { };
  Command.Color = Container->Style.BaseColor;
  Command.DstRect = Container->Rect;
  Command.Z = Container->Z;
  PushDrawCommand(CTX->CommandStack, &Command);
}

internal void
GuiDrawButton(context* CTX, element_id Id, container* Container) {
  draw_command Command = { };
  Command.DstRect = Container->Rect;
  Command.Z = Container->Z;
  b32 Hovered = (CTX->Active.Id == Id.Id);
  if (Hovered) {
    if (CTX->LeftMouseDown) {
      Command.Color = Container->Style.ActiveColor;
    } else {
      Command.Color = Container->Style.HotColor;
    }
  } else {
    Command.Color = Container->Style.BaseColor;
  }
  PushDrawCommand(CTX->CommandStack, &Command);
}

internal void
GuiBegin(context* CTX, rect Rect) {
  CTX->NumIDs = 1;
  CTX->NumContainers = 1;

  container Container = {};
  Container.Rect = Rect;
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
PanelBegin(context* CTX, 
           const char* Name,
           layout Layout = layout::Vertical,
           u32 Flags = 0) 
{
  GuiPushId(CTX, (const void*) Name, (u32)strlen(Name));
  container* Parent = &CTX->Containers[CTX->NumContainers-1];
  container Container = CreateChildContainer(CTX, Parent);
  Container.Style = CTX->Style;
  Container.Layout = Layout;
  Container.Flags = Flags;
  GuiPushContainer(CTX, &Container);
}

internal void
PanelEnd(context* CTX) {
  container* Panel = GuiPopContainer(CTX);
  container* Parent = &CTX->Containers[CTX->NumContainers-1];
  Panel->Rect.w = Panel->Content.x
                  + Panel->Margin[(u32)margin::Left]
                  + Panel->Margin[(u32)margin::Right];
  Panel->Rect.h = Panel->Content.y
                  + Panel->Margin[(u32)margin::Top]
                  + Panel->Margin[(u32)margin::Bottom];

  if (Parent->Layout == layout::Horizontal) {
    Parent->Content.x += Panel->Content.x;
    Parent->Content.y = MAX(Parent->Content.y, Panel->Content.y);
  } else {
    Parent->Content.y += Panel->Content.y;
    Parent->Content.x = MAX(Parent->Content.x, Panel->Content.x);
  }
  GuiDrawPanel(CTX, Panel);
  GuiPopId(CTX);
}

internal void
Label(context* CTX, const char* Text) {
  u32 TextLen = (u32)strlen(Text);
  container* Parent = &CTX->Containers[CTX->NumContainers-1];
  f32 LeftMargin = Parent->Margin[(u32)margin::Left];
  f32 TopMargin = Parent->Margin[(u32)margin::Top];
  f32 LeftContent = Parent->Content.x;
  f32 TopContent = Parent->Content.y;
  vec2 Pos = vec2{ Parent->Rect.x + LeftMargin + LeftContent, 
                   Parent->Rect.y + TopMargin + TopContent };
  color Color = CTX->Style.TextColor;
  vec2 TextSize = GetTextSize(Text, TextLen, &CTX->FontData);
  Parent->Content.y += 
                  (Parent->Layout == layout::Vertical) * TextSize.y;
  Parent->Content.x += 
                  (Parent->Layout == layout::Horizontal) * TextSize.x;
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
  container* Parent = &CTX->Containers[CTX->NumContainers - 1];
  container Container = CreateChildContainer(CTX, 
                                             Parent,
                                             ButtonWidth,
                                             ButtonHeight,
                                             layout::Horizontal);
  Container.Style = CTX->Style;
  Container.Layout = layout::Horizontal;
  memcpy(Container.Margin, CTX->Style.ButtonMargin, sizeof(Container.Margin));
  GuiPushContainer(CTX, &Container);
  Label(CTX, Text);
  GuiPopContainer(CTX);

  GuiUpdateElementInput(CTX, Id, &Container);

  b32 Hovered = (CTX->Active.Id == Id.Id);

  if (CTX->LeftMousePressed && Hovered) {
    Clicked = true;
  }

  GuiDrawButton(CTX, Id, &Container);
  return (Clicked);
}

internal b32
ImageButton(context* CTX, image* Image) {
  return false;
}

} // namespace gui

#define GUI_H
#endif
