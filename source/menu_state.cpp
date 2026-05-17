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

   storyButton = Text::make(font, "STORY", 50.0f);
   endlessButton = Text::make(font, "ENDLESS", 50.0f);
   freePlayButton = Text::make(font, "FREE-PLAY", 50.0f);
   backButton = Text::make(font, "BACK", 50.0f);
   gameModeSelectionButtons.addElements({storyButton, endlessButton, freePlayButton, backButton});

   updateResponsiveness();
}

void MenuState::updateResponsiveness() {
   float cr = getCubicRatio();

   playButton->position      = storyButton->position    = {cr * 125.0f, cr * 300.0f};
   editorButton->position    = endlessButton->position  = {cr * 125.0f, cr * 400.0f};
   customizeButton->position = freePlayButton->position = {cr * 125.0f, cr * 500.0f};
   optionsButton->position   = backButton->position     = {cr * 125.0f, cr * 600.0f};
   quitButton->position                                 = {cr * 125.0f, cr * 700.0f};
}

State *MenuState::change() {
   if (shouldCustomize) {
      return new CustomizeState();
   }
   else if (shouldPlayLevel) {
      // TODO: temp
      GameState *game = new GameState();
      game->setup(getLevel(0));
      return game;
   }

   return nullptr;
}

// Update

void MenuState::update() {
   switch (phase) {
   case Phase::title:             updateTitleState();             break;
   case Phase::gameModeSelection: updateGameModeSelectionState(); break;
   }
}

void MenuState::updateTitleState() {
   titleButtons.update();

   if (playButton->clicked) {
      phase = Phase::gameModeSelection;
   }

   if (editorButton->clicked) {

   }

   if (customizeButton->clicked) {
      shouldCustomize = true;
      fadingOut = true;
   }

   if (optionsButton->clicked) {

   }

   if (quitButton->clicked) {
      fadingOut = true;
   }
}

void MenuState::updateGameModeSelectionState() {
   gameModeSelectionButtons.update();

   if (storyButton->clicked) {
      shouldPlayLevel = true;
      fadingOut = true;
   }

   if (endlessButton->clicked) {

   }

   if (freePlayButton->clicked) {

   }

   if (backButton->clicked) {
      phase = Phase::title;
   }
}

// Render

void MenuState::render() {
   switch (phase) {
   case Phase::title:             renderTitleState();             break;
   case Phase::gameModeSelection: renderGameModeSelectionState(); break;
   }
}

void MenuState::renderTitleState() {
   Font font = getFont("slackey");
   float cr = getCubicRatio();
   
   drawTextButtonBackground(600.0f, RED);
   drawTextSemiCentered(font, {cr * 25.0f, cr * 100.0f}, "      ABYSS\n ASCENDANT 3", 80.0f, WHITE);
   titleButtons.render();

   if (titleButtons.anySelected()) {
      drawPointer(getTexture("lotus"), titleButtons.getSelectedElement()->position, true);
   }
}

void MenuState::renderGameModeSelectionState() {
   Font font = getFont("slackey");
   float cr = getCubicRatio();

   drawTextButtonBackground(600.0f, RED);
   drawTextSemiCentered(font, {cr * 25.0f, cr * 100.0f}, "  SELECT GAME\n       MODE", 80.0f, WHITE);
   gameModeSelectionButtons.render();

   if (gameModeSelectionButtons.anySelected()) {
      drawPointer(getTexture("lotus"), gameModeSelectionButtons.getSelectedElement()->position, true);
   }
}
