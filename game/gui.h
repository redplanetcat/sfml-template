#ifndef GUI_H

#define MAX_GUI_COMMANDS (256 * 1024)

namespace gui {

struct element_id {
  u32 id;
};

enum class command_type {
  Clip,
  Rect,
  Text,
  Image,
};

struct command {
   command_type Type;
   rect Rect;
   color Color;
   image Image;
};

struct container {
  i32 ZIndex;
  b32 Open;
};

struct context {
  element_id Hot;
  element_id Active;
  command CommandBuffer[MAX_GUI_COMMANDS];
  u32 NumCommands;
};

internal void
GuiInit(context* CTX) {

}

internal void 
GuiBegin(context* CTX) {

}

internal void
GuiUpdateInput(context* CTX, game_input* Input) {
  if (!Input->IsMouseButtonDown((u32)mouse_button::Left)) {
    CTX->Active = { };
  }
}

internal void
GuiEnd(context* CTX) {
  
}

internal void
PanelBegin(context* CTX) {

}

internal void
PanelEnd(context* CTX) {

}

internal void
Text(context* CTX, const char* Text) {

}

internal void
Image(context* CTX, image* Image) {

}

internal b32
Button(context* CTX, const char* Text) {
  return false;
}

internal b32
ImageButton(context* CTX, image* Image) {
  return false;
}

} // namespace gui

#define GUI_H
#endif
