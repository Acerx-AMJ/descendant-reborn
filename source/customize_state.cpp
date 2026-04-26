#include "customize_state.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "menu_state.hpp"

constexpr size_t extraButtons = 7;

CustomizeState::CustomizeState() {
   Shader shader = getShader("twocolor");
   primaryShaderLocation = GetShaderLocation(shader, "primary");
   secondaryShaderLocation = GetShaderLocation(shader, "secondary");

   Font font = getFont("slackey");
   Texture backTexture = getTexture("back"),
           buttonTexture = getTexture("button"),
           hiddenTexture = getTexture("hidden"),
           visibleTexture = getTexture("visible"),
           diceTexture = getTexture("dice");

   backButton = Button::make(backTexture, {}, "", 0.0f),
   skinTab = Button::make(buttonTexture, font, "SKIN", 50.0f),
   primaryTab = Button::make(buttonTexture, font, "PRIMARY", 50.0f),
   secondaryTab = Button::make(buttonTexture, font, "SECONDARY", 50.0f),
   hiddenButton = Button::make(hiddenTexture, {}, "", 0.0f),
   visibleButton = Button::make(visibleTexture, {}, "", 0.0f),
   diceButton = Button::make(diceTexture, {}, "", 0.0f);

   skinButtons.addElements({backButton->copy(), skinTab->copy(), primaryTab->copy(),
      secondaryTab->copy(), diceButton->copy(), visibleButton->copy()});
   colorButtons.addElements({backButton->copy(), skinTab->copy(), primaryTab->copy(),
      secondaryTab->copy(), diceButton->copy(), visibleButton->copy()});
   hiddenButtons.addElements({hiddenButton});
   noTabButtons.addElements({backButton, skinTab, primaryTab, secondaryTab, diceButton, visibleButton});

   updateResponsiveness();
}

CustomizeState::~CustomizeState() {

}

void CustomizeState::update() {
   if (backButton->clicked || IsKeyPressed(KEY_ESCAPE)) {
      fadingOut = true;
   }

   if (!visible) {
      hiddenButtons.update();

      if (hiddenButton->clicked || IsKeyPressed(KEY_H)) {
         visibleButton->texture = getTexture("visible");
         visible = true;
      }
      return;
   }

   // Update tab switching
   if (IsKeyPressed(KEY_LEFT_BRACKET)) {
      if (tab == Tab::skin)
         tab = Tab::none;
      else if (tab == Tab::primary)
         tab = Tab::skin;
      else if (tab == Tab::secondary)
         tab = Tab::primary;
      else
         tab = Tab::secondary;
   }

   if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
      if (tab == Tab::skin)
         tab = Tab::primary;
      else if (tab == Tab::primary)
         tab = Tab::secondary;
      else if (tab == Tab::secondary)
         tab = Tab::none;
      else
         tab = Tab::skin;
   }

   // Update buttons
   if (tab == Tab::skin) {
      skinButtons.update();
   }
   else if (tab != Tab::none) {
      colorButtons.update();
   }
   else {
      noTabButtons.update();
   }

   // Update corner buttons
   if (diceButton->clicked || IsKeyPressed(KEY_R)) {
      primaryColorID = rand() % getPlayerIconCount();
      primaryColorID = rand() % getPlayerColorCount();
      secondaryColorID = rand() % getPlayerColorCount();
   }

   if (visibleButton->clicked || IsKeyPressed(KEY_H)) {
      visibleButton->texture = getTexture("hidden");
      visible = false;
   }

   // Update tabs
   if (skinTab->clicked || IsKeyPressed(KEY_ONE)) {
      tab = (tab == Tab::skin ? Tab::none : Tab::skin);
      skinButtons.setIndex(extraButtons + skinID);
   }
   
   if (primaryTab->clicked || IsKeyPressed(KEY_TWO)) {
      tab = (tab == Tab::primary ? Tab::none : Tab::primary);
      colorButtons.setIndex(extraButtons + primaryColorID);
   }

   if (secondaryTab->clicked || IsKeyPressed(KEY_THREE)) {
      tab = (tab == Tab::secondary ? Tab::none : Tab::secondary);
      colorButtons.setIndex(extraButtons + secondaryColorID);
   }
}

void CustomizeState::render() {
   if (!visible) {
      hiddenButtons.render();
      return;
   }

   if (tab == Tab::none) {
      noTabButtons.render();
   }
   else if (tab == Tab::skin) {
      Shader shader = getShader("twocolor");
      Vector3 primary = getPlayerColor(primaryColorID);
      Vector3 secondary = getPlayerColor(secondaryColorID);

      SetShaderValue(shader, primaryShaderLocation, &primary, SHADER_UNIFORM_VEC3);
      SetShaderValue(shader, secondaryShaderLocation, &secondary, SHADER_UNIFORM_VEC3);

      BeginShaderMode(shader);
         for (size_t i = extraButtons; i < skinButtons.elements.size(); ++i) {
            skinButtons.getElement(i)->render();
         }
      EndShaderMode();

      for (size_t i = 1; i < extraButtons; ++i) {
         skinButtons.getElement(i)->render();
      }
   }
   else {
      colorButtons.render();
   }
}

void CustomizeState::fixedUpdate() {

}

void CustomizeState::updateResponsiveness() {
   float cr = getCubicRatio();

   backButton->position = {cr * 55.0f, cr * 55.0f};
   diceButton->position = {GetScreenWidth() - cr * 55.0f, cr * 55.0f};
   visibleButton->position = {cr * 55.0f, GetScreenHeight() - cr * 55.0f};
   hiddenButton->position = {cr * 55.0f, GetScreenHeight() - cr * 55.0f};
   backButton->size = diceButton->size = visibleButton->size = hiddenButton->size = {70.0f, 70.0f};

   skinTab->position = {GetScreenWidth() / 2.0f - 305.0f * cr, 70.0f * cr};
   primaryTab->position = {GetScreenWidth() / 2.0f, 70.0f * cr};
   secondaryTab->position = {GetScreenWidth() / 2.0f + 305.0f * cr, 70.0f * cr};
   skinTab->size = primaryTab->size = secondaryTab->size = {300.0f, 100.0f};
}

State *CustomizeState::change() {
   return new MenuState();
}
