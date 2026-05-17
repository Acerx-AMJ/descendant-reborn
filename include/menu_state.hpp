#pragma once
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
   Text *playButton, *editorButton, *customizeButton, *optionsButton, *quitButton, *storyButton,
      *endlessButton, *freePlayButton, *backButton;

   bool shouldPlayLevel = false;
   bool shouldCustomize = false;
};
