#include "menu_state.hpp"
#include "asset.hpp"
#include "customize_state.hpp"
#include "game_state.hpp"
#include <math.h>

MenuState::MenuState() {
   Font font = getFont("slackey");

   playButton = Text::make(font, "PLAY", 50.0f),
   editorButton = Text::make(font, "EDITOR", 50.0f),
   customizeButton = Text::make(font, "CUSTOMIZE", 50.0f),
   optionsButton = Text::make(font, "OPTIONS", 50.0f);
   quitButton = Text::make(font, "QUIT", 50.0f);

   titleButtons.addElements({playButton, editorButton, customizeButton, optionsButton, quitButton});
   updateResponsiveness();
}

void MenuState::update() {
   titleButtons.update();

   if (playButton->clicked) {
      phase = Phase::play;
      fadingOut = true;
   }

   if (editorButton->clicked) {

   }

   if (customizeButton->clicked) {
      phase = Phase::customize;
      fadingOut = true;
   }

   if (optionsButton->clicked) {

   }

   if (quitButton->clicked) {
      fadingOut = true;
   }
}

void MenuState::render() {
   Font font = getFont("slackey");
   float cr = getCubicRatio();
   
   DrawRectangleV({0.0f, 0.0f}, {500.0f * cr, GetScreenHeight() * 1.0f}, Fade(BLACK, 0.5f));
   drawTextSemiCentered(font, {cr * 25.0f, cr * 100.0f}, "DESCENDANT\n   REBORN", 80.0f, WHITE);
   titleButtons.render();

   if (titleButtons.anySelected()) {
      drawTextureCentered(getTexture("lotus"), {cr * 50.0f, titleButtons.getSelectedElement()->position.y}, cubicSize(65.0f + 10.0f * sin(GetTime() * 3.0f)), WHITE, GetTime() * 40.0f);
   }
}

void MenuState::fixedUpdate() {

}

void MenuState::updateResponsiveness() {
   float cr = getCubicRatio();

   playButton->position = {cr * 100.0f, cr * 300.0f};
   editorButton->position = {cr * 100.0f, cr * 400.0f};
   customizeButton->position = {cr * 100.0f, cr * 500.0f};
   optionsButton->position = {cr * 100.0f, cr * 600.0f};
   quitButton->position = {cr * 100.0f, cr * 700.0f};
}

State *MenuState::change() {
   if (phase == Phase::customize) {
      return new CustomizeState();
   }
   else if (phase == Phase::play) {
      return new GameState();
   }

   return nullptr;
}
