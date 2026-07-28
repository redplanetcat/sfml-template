#ifndef MENU_H

void DrawMenu(gui::context* CTX) {
  gui::GuiBegin(CTX);

  CTX->Style.BaseColor = {200, 200, 200, 255};

  gui::PanelBegin(CTX, "Panel", gui::layout::Vertical, (u32)gui::container_flags::Expand);

    gui::Label(CTX, "Menu");

    gui::PanelBegin(CTX, "Panel", gui::layout::Horizontal, (u32)gui::container_flags::Expand);

      CTX->Style.BaseColor = {255, 0, 0, 255};

      gui::PanelBegin(CTX, "PanelInner1");

        CTX->Style.BaseColor = {100, 100, 100, 255};
        if (gui::Button(CTX, "Button 1")) {
          printf("Button 1 pressed\n");
        };
        if (gui::Button(CTX, "Button 2")) {
          printf("Button 2 pressed\n");
        };
        if (gui::Button(CTX, "Button 3")) {
          printf("Button 3 pressed\n");
        };
        if (gui::Button(CTX, "Button 4")) {
          printf("Button 4 pressed\n");
        };
        if (gui::Button(CTX, "Button 5")) {
          printf("Button 5 pressed\n");
        };
        if (gui::Button(CTX, "Button 6")) {
          printf("Button 6 pressed\n");
        };

      gui::PanelEnd(CTX);
    
      CTX->Style.BaseColor = {0, 255, 0, 255};
      
      gui::PanelBegin(CTX, "PanelInner2");

        CTX->Style.BaseColor = {100, 100, 100, 255};
        if (gui::Button(CTX, "Button 7")) {
          printf("Button 7 pressed\n");
        };
        if (gui::Button(CTX, "Button 8")) {
          printf("Button 8 pressed\n");
        };
        if (gui::Button(CTX, "Button 9")) {
          printf("Button 9 pressed\n");
        };
        if (gui::Button(CTX, "Button 10")) {
          printf("Button 10 pressed\n");
        };
        if (gui::Button(CTX, "Button 11")) {
          printf("Button 11 pressed\n");
        };
        if (gui::Button(CTX, "Button 12")) {
          printf("Button 12 pressed\n");
        };

      gui::PanelEnd(CTX);

    gui::PanelEnd(CTX);

    gui::Image(CTX, nullptr);
    if (gui::ImageButton(CTX, nullptr)) {

    };

  gui::PanelEnd(CTX);

  gui::GuiEnd(CTX);
}

#define MENU_H
#endif
