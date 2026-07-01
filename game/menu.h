#ifndef MENU_H

void DrawMenu(gui::context* CTX, platform_callbacks* Callbacks) {
  gui::GuiBegin(CTX);

  gui::PanelBegin(CTX);

  gui::Text(CTX, "Menu");
  if (gui::Button(CTX, "Button")) {

  };

  gui::Image(CTX, nullptr);
  if (gui::ImageButton(CTX, nullptr)) {

  };

  gui::PanelEnd(CTX);

  gui::GuiEnd(CTX);
}

#define MENU_H
#endif
