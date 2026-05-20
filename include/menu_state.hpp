#pragma once
#include "data.hpp"
#include "navig.hpp"
#include "state.hpp"

struct MenuState: public State {
   MenuState();
   ~MenuState() = default;

   void updateResponsiveness() override;
   State *change() override;

   void update() override;
   void updateTitleState();
   void updateGameModeSelectionState();

   void render() override;
   void renderTitleState();
   void renderGameModeSelectionState();

private:

   enum class Phase {title, gameModeSelection};
   Phase phase = Phase::title;

   Navigation titleButtons, gameModeSelectionButtons;
   Text *playButton, *editorButton, *customizeButton, *optionsButton, *quitButton, *endlessButton,
      *freePlayButton, *backButton;
   Text *chapterButtons[chapterCount];

   TextureAA3 starTexture, timerTexture;
   std::string chapterInfo;
   size_t chapterID = 0;
   bool chapterUnlocked = false;

   bool endlessSelected = false;
   bool freePlaySelected = false;
   bool shouldPlayLevel = false;
   bool shouldCustomize = false;
};
