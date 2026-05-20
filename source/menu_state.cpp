#include "menu_state.hpp"
#include "asset.hpp"
#include "customize_state.hpp"
#include "file.hpp"
#include "game_state.hpp"
#include <math.h>

constexpr size_t entriesPerColumn = 10;
constexpr size_t entryColumns = 3;
constexpr size_t entriesPerPage = entriesPerColumn * entryColumns;

MenuState::MenuState() {
   Font font = getFont("slackey");

   playButton = Text::make(font, "PLAY", 50.0f),
   editorButton = Text::make(font, "EDITOR", 50.0f),
   customizeButton = Text::make(font, "CUSTOMIZE", 50.0f),
   optionsButton = Text::make(font, "OPTIONS", 50.0f);
   quitButton = Text::make(font, "QUIT", 50.0f);
   titleButtons.addElements({playButton, editorButton, customizeButton, optionsButton, quitButton});

   for (size_t i = 0; i < chapterCount; ++i) {
      if (i != 0 && !getChapterData(i - 1).completed) {
         chapterButtons[i] = Text::make(font, toRomanNumeral(i+1) + ": ???", 50.0f);
         chapterButtons[i]->disabled = true;
      }
      else {
         chapterButtons[i] = Text::make(font, toRomanNumeral(i+1) + ": " + getChapterCodeName(i), 50.0f);
      }
   }

   endlessButton = Text::make(font, "ENDLESS", 50.0f);
   freePlayButton = Text::make(font, "FREE-PLAY", 50.0f);
   backButton = Text::make(font, "BACK", 50.0f);
   gameModeSelectionButtons.addElements({chapterButtons[0], chapterButtons[1], chapterButtons[2],
      chapterButtons[3], chapterButtons[4], chapterButtons[5], chapterButtons[6], endlessButton,
      freePlayButton, backButton});

   initAnimationIfExists(starTexture, "star");
   initAnimationIfExists(timerTexture, "timer");

   updateResponsiveness();
}

void MenuState::updateResponsiveness() {
   float cr = getCubicRatio();

   playButton->position = {cr * 125.0f, cr * 300.0f};
   editorButton->position = {cr * 125.0f, cr * 400.0f};
   customizeButton->position = {cr * 125.0f, cr * 500.0f};
   optionsButton->position = {cr * 125.0f, cr * 600.0f};
   quitButton->position = {cr * 125.0f, cr * 700.0f};

   for (size_t i = 0; i < chapterCount; ++i) {
      chapterButtons[i]->position = {cr * 125.0f, cr * (225.0f + 75.0f * i)};
   }

   endlessButton->position = {cr * 125.0f, cr * 750.0f};
   freePlayButton->position = {cr * 125.0f, cr * 825.0f};
   backButton->position = {cr * 125.0f, cr * 900.0f};
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

   for (size_t i = 0; i < chapterCount; ++i) {
      if (chapterButtons[i]->hovering) {
         chapterInfo = chapterButtons[i]->text;
         chapterUnlocked = !chapterButtons[i]->disabled;
         chapterID = i;
      }

      if (chapterButtons[i]->clicked) {
         shouldPlayLevel = true;
         fadingOut = true;
      }
   }
   endlessSelected = false;
   freePlaySelected = false;

   if (gameModeSelectionButtons.anySelected() && gameModeSelectionButtons.index <= chapterCount) {
      size_t i = gameModeSelectionButtons.index - 1;
      chapterInfo = chapterButtons[i]->text;
      chapterUnlocked = !chapterButtons[i]->disabled;
      chapterID = i;
   }
   else if (endlessButton->hovering) {
      chapterInfo = endlessButton->text;
      endlessSelected = true;
      chapterUnlocked = false;
   }
   else if (freePlayButton->hovering) {
      chapterInfo = freePlayButton->text;
      freePlaySelected = true;
      chapterUnlocked = false;
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

   drawTextCentered(font, {cr * 1260.0f, cr * 150.0f}, chapterInfo.c_str(), 80.0f, WHITE);
   if (!endlessSelected && !freePlaySelected && chapterUnlocked) {
      ChapterData &data = getChapterData(chapterID);
      
      drawTextureAnimatedCentered(starTexture, {cr * 1000.0f, cr * 300.0f}, cubicSize(50.0f), WHITE);
      drawTextSemiCentered(font, {cr * 1040.0f, cr * 300.0f}, (data.completed ? TextFormat("%d/%d", data.starCount, data.levels.size() * 3) : TextFormat("%d/???", data.starCount)), 50.0f, WHITE);

      drawTextureAnimatedCentered(timerTexture, {cr * 1440.0f, cr * 300.0f}, cubicSize(50.0f), WHITE);
      drawTextSemiCentered(font, {cr * 1480.0f, cr * 300.0f}, (data.totalTime != std::numeric_limits<float>::max() ? TextFormat("%05.2f", data.totalTime) : "--.--"), 50.0f, WHITE);
   }

   if (gameModeSelectionButtons.anySelected()) {
      drawPointer(getTexture("lotus"), gameModeSelectionButtons.getSelectedElement()->position, true);
   }
}
