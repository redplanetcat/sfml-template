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
  Visible,
  Expand,
};

struct container {
  rect Rect;
  f32 Margin[4];
  i32 ZIndex;
  u32 Flags;
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
  command_stack* CommandStack;
  u32 ZBase;
};

internal void
GuiInit(context* CTX, command_stack* CommandStack) {
  CTX->NumIDs = 1;
  CTX->NumContainers = 1;
  CTX->ZBase = UINT32_MAX / 2;
  CTX->CommandStack = CommandStack;
}

internal void
GuiBegin(context* CTX) {

}

internal void
GuiUpdateInputState(context* CTX, game_input* Input) {
  if (!Input->IsMouseButtonDown((u32)mouse_button::Left)) {
    CTX->Active = {};
  }
}

internal void
GuiUpdateElementInput(context* CTX, element_id Id, rect Rect) {

}

internal element_id
GuiGetId(context* CTX, const void* Data, u32 Size) {
  element_id Id = {};
  Id.Id = (CTX->NumIDs > 0) ? ctx->IDs[CTX->NumIDs - 1] : HASH_INITIAL;
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
  CTX->Conainers[CTX->NumContainers] = *Container;
  CTX->NumContainers++;
}

internal void
GuiPopContainer(context* CTX) {
  Assert(CTX->NumContainers > 1);
  CTX->NumContainers--;
}

internal void
GuiDrawPanel(context* CTX, element_id Id, rect PanelRect) {
  draw_command Command = { };
  Command.Texture = E.Texture;
  Command.Shader = E.Shader;
  Command.Z = E.Z;
  Command.Color = E.Color;
  Command.Rotation = E.Rot;
  vec2 ScaledSize = vec2{ E.Size.x * (1.f + E.Scale), E.Size.y * (1.f + E.Scale) };
  vec2 Position = vec2{ E.Pos.x - ScaledSize.x / 2.f, E.Pos.y - ScaledSize.y / 2.f };
  if (E.Flags & (u32)entity_flags::Flip) {
    Command.Flags |= (u32)draw_command_flags::Flip;
  }
  Command.Rect = rect{ Position.x, Position.y,
                       ScaledSize.x, ScaledSize.y };
  PushDrawCommand(CTX->CommandStack, &Command);

}

internal void
GuiDrawButton(context* CTX, element_id Id, rect ButtonRect) {

}

internal void
GuiEnd(context* CTX) {
  
}

internal void
PanelBegin(context* CTX, const char* Name) {
  GuiPushId(CTX, (const void*) Name, strlen(Name));

}

internal void
PanelEnd(context* CTX) {
  GuiPopId(CTX);
}

internal void
Text(context* CTX, const char* Text) {

}

internal void
Image(context* CTX, image* Image) {

}

internal b32
Button(context* CTX, const char* Text) {
  b32 Clicked = false;
  element_id Id = GuiGetId(ctx, (const void*)Text, strlen(Text));

  GuiUpdateElementInput(CTX, Id, ButtonRect);
  
  if (CTX->Active == Id) {
    Clicked = true;
  }

  GuiDrawButton(CTX, Id, ButtonRect);
  return (Clicked);
}

internal b32
ImageButton(context* CTX, image* Image) {
  return false;
}

} // namespace gui

#define GUI_H
#endif
