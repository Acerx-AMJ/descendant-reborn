#include "menu_state.hpp"
#include "asset.hpp"
#include "customize_state.hpp"

MenuState::MenuState() {
   Texture buttonTexture = getTexture("button");
   Font font = getFont("slackey");

   Button *playButton = Button::make(buttonTexture, font, "PLAY", 50.0f),
          *editorButton = Button::make(buttonTexture, font, "EDITOR", 50.0f),
          *customizeButton = Button::make(buttonTexture, font, "CUSTOMIZE", 50.0f),
          *quitButton = Button::make(buttonTexture, font, "QUIT", 50.0f);

   playButton->size = editorButton->size = customizeButton->size = quitButton->size
      = {300.0f, 100.0f};

   titleButtons.addElements({playButton, editorButton, customizeButton, quitButton});
   updateResponsiveness();
}

void MenuState::update() {
   titleButtons.update();

   // Play button
   if (titleButtons.getButton(1)->clicked) {

   }

   // Editor button
   if (titleButtons.getButton(2)->clicked) {

   }

   // Customize button
   if (titleButtons.getButton(3)->clicked) {
      phase = Phase::customize;
      fadingOut = true;
   }

   // Quit button
   if (titleButtons.getButton(4)->clicked) {
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
   // float wr = getWidthRatio();
   float hr = getHeightRatio();
   // float cr = getCubicRatio();

   titleButtons.getButton(1)->position = getScreenCenterOffset({0.0f, -100.0f * hr});
   titleButtons.getButton(2)->position = getScreenCenterOffset({0.0f, 20.0f * hr});
   titleButtons.getButton(3)->position = getScreenCenterOffset({0.0f, 140.0f * hr});
   titleButtons.getButton(4)->position = getScreenCenterOffset({0.0f, 260.0f * hr});
}

State *MenuState::change() {
   if (phase == Phase::customize) {
      return new CustomizeState();
   }

   return nullptr;
}
