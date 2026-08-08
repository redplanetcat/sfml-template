#ifndef MENU_H

void DrawMenu(gui::context* CTX, u8* GameMode) {
  gui::GuiBegin(CTX);

  CTX->Style.BaseColor = {200, 200, 200, 255};

  gui::PanelBegin(CTX, "PanelVerticalOuter", gui::layout::Vertical); 

    gui::Label(CTX, "Menu");

    b32 InfiniteApplesOn = (*GameMode & 1 << (u32)game_mode::InfiniteApples);
    b32 ManyApplesOn = (*GameMode & 1 << (u32)game_mode::ManyApples);
    b32 SpeedUpOn = (*GameMode & 1 << (u32)game_mode::SpeedUpOnScore);

    static char GameModeString[64];
    snprintf(GameModeString, sizeof(GameModeString), 
            "Game Mode: %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(*GameMode));
    gui::Label(CTX, GameModeString);

    static char InfiniteApplesString[64];
    static char ManyApplesString[64];
    static char SpeedUpString[64];
    static char OnString[] = "ON";
    static char OffString[] = "OFF";
    static char* StateString;
    StateString = InfiniteApplesOn ? OnString : OffString;
    snprintf(InfiniteApplesString, sizeof(InfiniteApplesString), 
              "Infinite Apples: %s", StateString);
    StateString = ManyApplesOn ? OnString : OffString;
    snprintf(ManyApplesString, sizeof(ManyApplesString), 
              "Many Apples: %s", StateString);
    StateString = SpeedUpOn ? OnString : OffString;
    snprintf(SpeedUpString, sizeof(SpeedUpString), "Speed Up: %s", StateString);

    gui::Label(CTX, InfiniteApplesString);
    gui::Label(CTX, ManyApplesString);
    gui::Label(CTX, SpeedUpString);


    gui::PanelBegin(CTX, "PanelHorizontal", gui::layout::Horizontal);

      CTX->Style.BaseColor = {255, 0, 0, 255};

      gui::PanelBegin(CTX, "PanelInner1");

        CTX->Style.BaseColor = {100, 100, 100, 255};
        if (gui::Button(CTX, "Infinite Apples")) {
          *GameMode ^= 1 << (u32)game_mode::InfiniteApples;
        };
        if (gui::Button(CTX, "Many Apples")) {
          *GameMode ^= 1 << (u32)game_mode::ManyApples;
        };

      gui::PanelEnd(CTX); // PanelInner1

      CTX->Style.BaseColor = {0, 255, 0, 255};

      gui::PanelBegin(CTX, "PanelInner2");

        CTX->Style.BaseColor = {100, 100, 100, 255};
        if (gui::Button(CTX, "Speed Up")) {
          *GameMode ^= 1 << (u32)game_mode::SpeedUpOnScore;
        };

      gui::PanelEnd(CTX); // PanelInnner2

    gui::PanelEnd(CTX); // PanelHorizontal

    //gui::PanelBegin(CTX, "PanelStartGame");

      if (gui::Button(CTX, "Start Game!")) {

      }

    //gui::PanelEnd(CTX); // StartGamePanel

    gui::Image(CTX, nullptr);
    if (gui::ImageButton(CTX, nullptr)) {

    };

  gui::PanelEnd(CTX); // Vertical Outer

  gui::GuiEnd(CTX);
}

#define MENU_H
#endif
