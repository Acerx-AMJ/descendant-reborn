#include "menu_state.hpp"
#include "asset.hpp"
#include "customize_state.hpp"
#include "game_state.hpp"

MenuState::MenuState() {
   Texture buttonTexture = getTexture("button");
   Font font = getFont("slackey");

   playButton = Button::make(buttonTexture, {300.0f, 100.0f}, font, "PLAY", 50.0f),
   editorButton = Button::make(buttonTexture, {300.0f, 100.0f}, font, "EDITOR", 50.0f),
   customizeButton = Button::make(buttonTexture, {300.0f, 100.0f}, font, "CUSTOMIZE", 50.0f),
   quitButton = Button::make(buttonTexture, {300.0f, 100.0f}, font, "QUIT", 50.0f);

   titleButtons.addElements({playButton, editorButton, customizeButton, quitButton});
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

   if (quitButton->clicked) {
      fadingOut = true;
   }
}

void MenuState::render() {
   Font font = getFont("slackey");
   drawTextCentered(font, getScreenCenterOffset({0.0f, -250.0f * getHeightRatio()}), "DESCENDANT REBORN", 120.0f, WHITE);

   titleButtons.render();

   if (titleButtons.anySelected()) {
      Button *button = titleButtons.getSelectedButton();
      float scale = getCubicRatio() * button->scale;
      drawTextureCentered(getTexture("button_pointer"), button->position, {318.75f * scale, 106.25f * scale}, WHITE);
   }
}

void MenuState::fixedUpdate() {

}

void MenuState::updateResponsiveness() {
   float cr = getCubicRatio();

   playButton->position = getScreenCenterOffset({0.0f, -100.0f * cr});
   editorButton->position = getScreenCenterOffset({0.0f, 20.0f * cr});
   customizeButton->position = getScreenCenterOffset({0.0f, 140.0f * cr});
   quitButton->position = getScreenCenterOffset({0.0f, 260.0f * cr});
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
