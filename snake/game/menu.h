#ifndef MENU_H
internal void
DrawHighScores(gui::context* CTX, hashmap* HighScores) {
  struct hashmap_node* ScoreNode = NULL;
  u32 ScoreIndex = 0;
  while (HashmapIterNodes(HighScores, &ScoreIndex, &ScoreNode)) {
    char ScoreBuf[256];
    char* ScoreName = &HighScores->Keys[HighScores->KeySize * ScoreNode->KeyIdx];
    snprintf(ScoreBuf, 256, "%s: %u", ScoreName, *((u32*)ScoreNode->Value));
    gui::Label(CTX, ScoreBuf);
  }
}

internal void 
DrawMenu(gui::context* CTX, u8* GameMode, game_scene* Scene, hashmap* HighScores) {
  gui::GuiBegin(CTX, rect{64, 64, 128, 64});

  CTX->Style.BaseColor = {200, 200, 200, 20};

  gui::PanelBegin(CTX, "PanelVerticalOuter", gui::layout::Vertical); 

    gui::Label(CTX, "Show/Hide Menu: F10");

    if (*GameMode & 1 << (u8)game_mode::MenuHidden) {
      PanelEnd(CTX);
      return;
    }

    b32 InfiniteApplesOn = (*GameMode & 1 << (u8)game_mode::InfiniteApples);
    b32 ManyApplesOn = (*GameMode & 1 << (u8)game_mode::ManyApples);
    b32 SpeedUpOn = (*GameMode & 1 << (u8)game_mode::SpeedUpOnScore);

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

      gui::PanelBegin(CTX, "PanelInner1");

        if (gui::Button(CTX, "Infinite Apples")) {
          *GameMode ^= 1 << (u8)game_mode::InfiniteApples;
        };
        if (gui::Button(CTX, "Many Apples")) {
          *GameMode ^= 1 << (u8)game_mode::ManyApples;
        };

      gui::PanelEnd(CTX); // PanelInner1

      gui::PanelBegin(CTX, "PanelInner2");

        if (gui::Button(CTX, "Speed Up")) {
          *GameMode ^= 1 << (u8)game_mode::SpeedUpOnScore;
        };

      gui::PanelEnd(CTX); // PanelInnner2

    gui::PanelEnd(CTX); // PanelHorizontal


    if (gui::Button(CTX, "Start Game!")) {
      *Scene = game_scene::Game;
      *GameMode |= (1 << (u8)game_mode::MenuHidden);
    }
    
    gui::PanelBegin(CTX, "PanelScores", gui::layout::Vertical);

      DrawHighScores(CTX, HighScores);

    gui::PanelEnd(CTX); // PanelScores


  gui::PanelEnd(CTX); // Vertical Outer

  gui::GuiEnd(CTX);
}

internal void 
WinMenu(gui::context* CTX, u8* GameMode, hashmap* HighScores, 
        u32 PlayerScore, b32* ScoreSubmitted, platform_callbacks* Callbacks) {
  gui::GuiBegin(CTX, rect{128, 128, 128, 64});

  CTX->Style.BaseColor = {200, 200, 200, 20};

  gui::PanelBegin(CTX, "PanelVerticalOuter", gui::layout::Vertical); 

    gui::Label(CTX, "You won!");

    if (!*ScoreSubmitted) {
      local_persist char NameBuffer[64] = "Player";

      gui::PanelBegin(CTX, "PanelNameInput", gui::layout::Horizontal);

        gui::InputText(CTX, NameBuffer, 64);

        if (gui::Button(CTX, "Submit")) {
          HashmapInsert(HighScores, (const void*)NameBuffer, 
                        (u32)strnlen(NameBuffer, 64), 
                        (const void*)&PlayerScore);
          SortLeaderboard(HighScores);
          SaveLeaderboardToFile(HighScores, "Resources/Scores.txt", Callbacks);
          *ScoreSubmitted = true;
        }

      gui::PanelEnd(CTX); // PanelNameInput
    }
    
    gui::PanelBegin(CTX, "PanelScores", gui::layout::Vertical);

      DrawHighScores(CTX, HighScores);

    gui::PanelEnd(CTX); // PanelScores


    if (gui::Button(CTX, "Restart")) {
      *GameMode |= (1 << (u8)game_mode::NeedsRestart);
    }

  gui::PanelEnd(CTX);

  gui::GuiEnd(CTX);
}

#define MENU_H
#endif
