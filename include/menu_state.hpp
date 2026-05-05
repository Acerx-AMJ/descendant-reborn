#pragma once
#include "navig.hpp"
#include "state.hpp"

struct MenuState: public State {
   MenuState();
   ~MenuState() = default;

   void update() override;
   void render() override;
   void fixedUpdate() override;
   void updateResponsiveness() override;
   State *change() override;

private:

   enum class Phase {title, chapterSelection, levelSelection, play, editor, customize, quit};
   Phase phase = Phase::title;

   Navigation titleButtons;
   Text *playButton, *editorButton, *customizeButton, *optionsButton, *quitButton;
};
