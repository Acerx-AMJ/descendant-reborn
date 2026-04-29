#include "customize_state.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "menu_state.hpp"

constexpr Rectangle bounds = {0.0f, 0.0f, 2000.0f, 1250.0f};
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

   backButton = Button::make(backTexture, {70.0f, 70.0f}),
   skinTab = Button::make(buttonTexture, {300.0f, 100.0f}, font, "SKIN", 50.0f),
   primaryTab = Button::make(buttonTexture, {300.0f, 100.0f}, font, "PRIMARY", 50.0f),
   secondaryTab = Button::make(buttonTexture, {300.0f, 100.0f}, font, "SECONDARY", 50.0f),
   hiddenButton = Button::make(hiddenTexture, {70.0f, 70.0f}),
   visibleButton = Button::make(visibleTexture, {70.0f, 70.0f}),
   diceButton = Button::make(diceTexture, {70.0f, 70.0f});

   skinButtons.addElements({backButton->copy(), skinTab->copy(), primaryTab->copy(),
      secondaryTab->copy(), visibleButton->copy(), diceButton->copy(),});
   colorButtons.addElements({backButton->copy(), skinTab->copy(), primaryTab->copy(),
      secondaryTab->copy(), visibleButton->copy(), diceButton->copy(),});
   hiddenButtons.addElements({hiddenButton});
   noTabButtons.addElements({backButton, skinTab, primaryTab, secondaryTab, visibleButton, diceButton});

   for (const std::string &icon: getPlayerIconContainer()) {
      TextureRect *rect = TextureRect::make(getTexture(icon), {70.0f, 70.0f});
      skinButtons.addElement(rect);
   }

   for (const Vector3 &color: getPlayerColorContainer()) {
      Color translated {
         static_cast<unsigned char>(color.x * 255.0f),
         static_cast<unsigned char>(color.y * 255.0f),
         static_cast<unsigned char>(color.z * 255.0f),
         255
      };
      TextureRect *rect = TextureRect::make(translated, {70.0f, 70.0f});
      colorButtons.addElement(rect);
   }

   updateResponsiveness();

   CustomizationData data = getCustomizationData();
   player.position = getOrigin({bounds.width, bounds.height});
   player.iconID = data.iconID;
   player.primaryColorID = data.primaryColorID;
   player.secondaryColorID = data.secondaryColorID;
   player.init(bounds);

   camera.init(&player, bounds, player.position, 1.0f, 0.1f, 0.1f, 0.25f, 4.0f);
}

CustomizeState::~CustomizeState() {
   saveCustomizationData({player.iconID, player.primaryColorID, player.secondaryColorID});
}

void CustomizeState::update() {
   // Camera
   float scrollDelta = GetMouseWheelMove();
   if (scrollDelta >= 0.5f) {
      camera.targetZoom -= GetFrameTime() * 6.0f;
   }
   else if (scrollDelta <= -0.5f) {
      camera.targetZoom += GetFrameTime() * 6.0f;
   }

   if (IsKeyPressed(KEY_EQUAL)) {
      camera.targetZoom -= 0.5f;
   }
   else if (IsKeyPressed(KEY_MINUS)) {
      camera.targetZoom += 0.5f;
   }

   if (backButton->clicked || handleKeyPressWithSound(KEY_ESCAPE)) {
      if (visible && tab != Tab::none) {
         tab = Tab::none;
      }
      else {
         fadingOut = true;
      }
   }

   if (!visible) {
      hiddenButtons.update();

      if (hiddenButton->clicked || handleKeyPressWithSound(KEY_H)) {
         // 5 is visible button
         skinButtons.setIndex(5);
         colorButtons.setIndex(5);
         noTabButtons.setIndex(5);
         visibleButton->texture = getTexture("visible");
         visible = true;
      }
      return;
   }

   // Update tab switching
   Tab lastTab = tab;
   if (handleKeyPressWithSound(KEY_LEFT_BRACKET)) {
      if (tab == Tab::skin)
         tab = Tab::none;
      else if (tab == Tab::primary)
         tab = Tab::skin;
      else if (tab == Tab::secondary)
         tab = Tab::primary;
      else
         tab = Tab::secondary;
   }

   if (handleKeyPressWithSound(KEY_RIGHT_BRACKET)) {
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
      for (size_t i = extraButtons; i < skinButtons.elements.size(); ++i) {
         TextureRect *rect = skinButtons.getTextureRect(i);
         if (rect->clicked) {
            player.iconID = rect->ID;
            skinButtons.setIndex(i);
         }
      }
   }
   else if (tab != Tab::none) {
      colorButtons.update();
      for (size_t i = extraButtons; i < colorButtons.elements.size(); ++i) {
         TextureRect *rect = colorButtons.getTextureRect(i);
         if (rect->clicked) {
            (tab == Tab::primary ? player.primaryColorID : player.secondaryColorID) = rect->ID;
            colorButtons.setIndex(i);
         }
      }
   }
   else {
      noTabButtons.update();
   }

   // Update corner buttons
   if (diceButton->clicked || handleKeyPressWithSound(KEY_R)) {
      player.iconID = rand() % getPlayerIconCount();
      player.primaryColorID = rand() % getPlayerColorCount();
      player.secondaryColorID = rand() % getPlayerColorCount();

      skinButtons.setIndex(extraButtons + player.iconID);
      colorButtons.setIndex(extraButtons + (tab == Tab::primary ? player.primaryColorID : player.secondaryColorID));
   }

   if (visibleButton->clicked || handleKeyPressWithSound(KEY_H)) {
      hiddenButtons.setIndex(1);
      visibleButton->texture = getTexture("hidden");
      visible = false;
   }

   // Update tabs
   if (skinTab->clicked || handleKeyPressWithSound(KEY_ONE)) {
      tab = (tab == Tab::skin ? Tab::none : Tab::skin);
   }
   
   if (primaryTab->clicked || handleKeyPressWithSound(KEY_TWO)) {
      tab = (tab == Tab::primary ? Tab::none : Tab::primary);
   }

   if (secondaryTab->clicked || handleKeyPressWithSound(KEY_THREE)) {
      tab = (tab == Tab::secondary ? Tab::none : Tab::secondary);
   }

   if (lastTab != tab && tab == Tab::skin) {
      skinButtons.setIndex(extraButtons + player.iconID);
   }

   if (lastTab != tab && tab == Tab::primary) {
      colorButtons.setIndex(extraButtons + player.primaryColorID);
   }

   if (lastTab != tab && tab == Tab::secondary) {
      colorButtons.setIndex(extraButtons + player.secondaryColorID);
   }
}

void CustomizeState::render() {
   BeginMode2D(camera.camera);
      DrawRectanglePro(bounds, {0.0f, 0.0f}, 0.0f, DARKGRAY);
      player.render();
   EndMode2D();
   
   if (!visible) {
      hiddenButtons.render();
      return;
   }

   if (tab == Tab::none) {
      noTabButtons.render();
   }
   else if (tab == Tab::skin) {
      Shader shader = getShader("twocolor");
      Vector3 primary = getPlayerColor(player.primaryColorID);
      Vector3 secondary = getPlayerColor(player.secondaryColorID);

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

   Texture pointerTexture = getTexture("button_pointer");
   Texture smallPointerTexture = getTexture("button_pointer_small");
   
   if (tab == Tab::skin) {
      float scale = getCubicRatio() * skinTab->scale;
      drawTextureCentered(pointerTexture, skinTab->position, {318.75f * scale, 106.25f * scale}, WHITE);
   }
   else if (tab == Tab::primary) {
      float scale = getCubicRatio() * primaryTab->scale;
      drawTextureCentered(pointerTexture, primaryTab->position, {318.75f * scale, 106.25f * scale}, WHITE);
   }
   else if (tab == Tab::secondary) {
      float scale = getCubicRatio() * secondaryTab->scale;
      drawTextureCentered(pointerTexture, secondaryTab->position, {318.75f * scale, 106.25f * scale}, WHITE);
   }

   if (tab == Tab::skin && skinButtons.getSelectedIndex() >= extraButtons) {
      TextureRect *selected = skinButtons.getSelectedTextureRect();
      float scale = getCubicRatio() * selected->scale;
      drawTextureCentered(smallPointerTexture, selected->position, {74.375f * scale, 74.375f * scale}, WHITE);
   }

   if ((tab == Tab::primary || tab == Tab::secondary) && colorButtons.getSelectedIndex() >= extraButtons) {
      TextureRect *selected = colorButtons.getSelectedTextureRect();
      float scale = getCubicRatio() * selected->scale;
      drawTextureCentered(smallPointerTexture, selected->position, {74.375f * scale, 74.375f * scale}, WHITE);
   }
}

void CustomizeState::fixedUpdate() {
   player.update();
   camera.update();
}

void CustomizeState::updateResponsiveness() {
   float cr = getCubicRatio();

   camera.camera.offset = getScreenCenter();

   backButton->position = {cr * 55.0f, cr * 55.0f};
   visibleButton->position = {cr * 55.0f, GetScreenHeight() - cr * 55.0f};
   hiddenButton->position = {cr * 55.0f, GetScreenHeight() - cr * 55.0f};
   diceButton->position = {cr * 145.0f, GetScreenHeight() - cr * 55.0f};

   skinTab->position = {GetScreenWidth() / 2.0f - 305.0f * cr, 70.0f * cr};
   primaryTab->position = {GetScreenWidth() / 2.0f, 70.0f * cr};
   secondaryTab->position = {GetScreenWidth() / 2.0f + 305.0f * cr, 70.0f * cr};

   float positionX = cr * 50.0f;
   float positionY = cr * 215.0f;

   for (size_t i = extraButtons; i < skinButtons.elements.size(); ++i) {
      TextureRect *button = skinButtons.getTextureRect(i);
      button->ID = i - extraButtons;
      button->position = {positionX, positionY};

      positionX += 90.0f * cr;
      if (positionX >= GetScreenWidth() - 40.0f * cr) {
         positionX = 50.0f * cr;
         positionY += 90.0f * cr;
      }
   }

   positionX = cr * 50.0f;
   positionY = cr * 215.0f;

   for (size_t i = extraButtons; i < colorButtons.elements.size(); ++i) {
      TextureRect *button = colorButtons.getTextureRect(i);
      button->ID = i - extraButtons;
      button->position = {positionX, positionY};

      positionX += 90.0f * cr;
      if (positionX >= GetScreenWidth() - 40.0f * cr) {
         positionX = 50.0f * cr;
         positionY += 90.0f * cr;
      }
   }
}

State *CustomizeState::change() {
   return new MenuState();
}
