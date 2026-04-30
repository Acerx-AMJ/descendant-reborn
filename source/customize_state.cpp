#include "customize_state.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "input.hpp"
#include "menu_state.hpp"
#include <raymath.h>

constexpr Rectangle bounds = {0.0f, 0.0f, 2000.0f, 1250.0f};
constexpr size_t extraButtons = 9;
constexpr size_t entriesPerColumn = 15;
constexpr size_t entryColumns = 5;
constexpr size_t entriesPerPage = entriesPerColumn * entryColumns;

CustomizeState::CustomizeState() {
   Shader shader = getShader("twocolor");
   primaryShaderLocation = GetShaderLocation(shader, "primary");
   secondaryShaderLocation = GetShaderLocation(shader, "secondary");

   Font font = getFont("slackey");
   Texture backTexture = getTexture("back"),
           buttonTexture = getTexture("button"),
           textInputTexture = getTexture("text_input"),
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
   shadowButton = Button::make({}, {70.0f, 70.0f});
   nameInput = TextInput::make(textInputTexture, {280.0f, 70.0f}, font, "Input your name...", 24, 25.0f);

   skinButtons.addElements({backButton->copy(), skinTab->copy(), primaryTab->copy(), secondaryTab->copy(),
      visibleButton->copy(), diceButton->copy(), shadowButton->copy(), nameInput->copy()});
   colorButtons.addElements({backButton->copy(), skinTab->copy(), primaryTab->copy(), secondaryTab->copy(),
      visibleButton->copy(), diceButton->copy(), shadowButton->copy(), nameInput->copy()});
   hiddenButtons.addElements({hiddenButton});
   noTabButtons.addElements({backButton, skinTab, primaryTab, secondaryTab, visibleButton, diceButton,
      shadowButton, nameInput});

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
   player.shadowsEnabled = data.shadowsEnabled;
   player.init(bounds);

   originalName = nameInput->text = data.username;
   shadowButton->texture = getTexture(player.shadowsEnabled ? "shadows_enabled" : "shadows_disabled");
   camera.init(&player, bounds, player.position, 1.0f, 0.1f, 0.1f, 0.25f, 4.0f);
}

CustomizeState::~CustomizeState() {
   saveCustomizationData({player.iconID, player.primaryColorID, player.secondaryColorID, player.shadowsEnabled, originalName});
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
   bool wasActive = nameInput->active;

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

   if (wasActive && !nameInput->active) {
      if (nameInput->text.size() < 3) {
         nameInput->text = originalName;
      }
      else {
         originalName = nameInput->text;
      }
   }
   player.blockMovement = nameInput->active;

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

   if (shadowButton->clicked || handleKeyPressWithSound(KEY_V)) {
      player.shadowsEnabled = !player.shadowsEnabled;
      shadowButton->texture = getTexture(player.shadowsEnabled ? "shadows_enabled" : "shadows_disabled");
   }

   // Update pages

   if (skinButtons.anySelected() && skinButtons.getSelectedIndex() >= extraButtons) {
      skinPage = (skinButtons.getSelectedIndex() - extraButtons) / entriesPerPage;
   }

   if (colorButtons.anySelected() && colorButtons.getSelectedIndex() >= extraButtons) {
      colorPage = (colorButtons.getSelectedIndex() - extraButtons) / entriesPerPage;
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
         size_t max = std::min(skinButtons.elements.size(), extraButtons + entriesPerPage * (skinPage + 1));
         float offsetX = GetScreenWidth() * skinPage;
         
         for (size_t i = extraButtons + entriesPerPage * skinPage; i < max; ++i) {
            skinButtons.getElement(i)->position.x -= offsetX;
            skinButtons.getElement(i)->render();
            skinButtons.getElement(i)->position.x += offsetX;
         }
      EndShaderMode();

      for (size_t i = 1; i < extraButtons; ++i) {
         skinButtons.getElement(i)->render();
      }
   }
   else {
      size_t max = std::min(colorButtons.elements.size(), extraButtons + entriesPerPage * (colorPage + 1));
      float offsetX = GetScreenWidth() * colorPage;

      for (size_t i = extraButtons + entriesPerPage * colorPage; i < max; ++i) {
         colorButtons.getElement(i)->position.x -= offsetX;
         colorButtons.getElement(i)->render();
         colorButtons.getElement(i)->position.x += offsetX;
      }

      for (size_t i = 1; i < extraButtons; ++i) {
         colorButtons.getElement(i)->render();
      }
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
      // 64/y = 68/x; x = 68y/64; x = 1.0625y
      // where 64 is the default texture size and 68 is the texture size of the button pointer,
      // and where y is the size of the button, and x is the size of the pointer
      float offsetX = GetScreenWidth() * skinPage;
      float scale = getCubicRatio() * selected->scale * 1.0625f;
      drawTextureCentered(smallPointerTexture, {selected->position.x - offsetX, selected->position.y}, Vector2Scale(selected->size, scale), WHITE);
   }

   if ((tab == Tab::primary || tab == Tab::secondary) && colorButtons.getSelectedIndex() >= extraButtons) {
      TextureRect *selected = colorButtons.getSelectedTextureRect();
      float offsetX = GetScreenWidth() * colorPage;
      float scale = getCubicRatio() * selected->scale * 1.0625f;
      drawTextureCentered(smallPointerTexture, {selected->position.x - offsetX, selected->position.y}, Vector2Scale(selected->size, scale), WHITE);
   }
   DrawText(TextFormat("ip: %lu cp: %lu", skinPage, colorPage), 50, 50, 50, RED);
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
   shadowButton->position = {cr * 235.0f, GetScreenHeight() - cr * 55.0f};
   nameInput->position = {cr * 430.0f, GetScreenHeight() - cr * 55.0f};

   skinTab->position = {GetScreenWidth() / 2.0f - 305.0f * cr, 70.0f * cr};
   primaryTab->position = {GetScreenWidth() / 2.0f, 70.0f * cr};
   secondaryTab->position = {GetScreenWidth() / 2.0f + 305.0f * cr, 70.0f * cr};

   float workingWidth = GetScreenWidth() - 200.0f * cr;
   float singleEntrySizeWithPadding = workingWidth / entriesPerColumn;
   float singleEntrySize = singleEntrySizeWithPadding * (6.0f / 7.0f);
   float singleEntrySizeTranslated = singleEntrySize / cr;

   float positionX = cr * 100.0f + singleEntrySize / 2.0f;
   float positionY = cr * 215.0f;

   for (size_t i = extraButtons; i < skinButtons.elements.size(); ++i) {
      TextureRect *button = skinButtons.getTextureRect(i);
      button->ID = i - extraButtons;
      button->position = {positionX, positionY};
      button->size = {singleEntrySizeTranslated, singleEntrySizeTranslated};

      positionX += singleEntrySizeWithPadding;
      size_t index = i - extraButtons + 1;
      
      if (index % entriesPerColumn == 0) {
         float screens = GetScreenWidth() * size_t(index / entriesPerPage);
         positionX = cr * 100.0f + singleEntrySize / 2.0f + screens;
         positionY += singleEntrySizeWithPadding;
      }

      if (index % entriesPerPage == 0) {
         float screens = GetScreenWidth() * size_t(index / entriesPerPage);
         positionX = cr * 100.0f + singleEntrySize / 2.0f + screens;
         positionY = cr * 215.0f;
      }
   }

   positionX = cr * 100.0f + singleEntrySize / 2.0f;
   positionY = cr * 215.0f;

   for (size_t i = extraButtons; i < colorButtons.elements.size(); ++i) {
      TextureRect *button = colorButtons.getTextureRect(i);
      button->ID = i - extraButtons;
      button->position = {positionX, positionY};
      button->size = {singleEntrySizeTranslated, singleEntrySizeTranslated};

      positionX += singleEntrySizeWithPadding;
      size_t index = i - extraButtons + 1;
      
      if (index % entriesPerColumn == 0) {
         float screens = GetScreenWidth() * size_t(index / entriesPerPage);
         positionX = cr * 100.0f + singleEntrySize / 2.0f + screens;
         positionY += singleEntrySizeWithPadding;
      }

      if (index % entriesPerPage == 0) {
         float screens = GetScreenWidth() * size_t(index / entriesPerPage);
         positionX = cr * 100.0f + singleEntrySize / 2.0f + screens;
         positionY = cr * 215.0f;
      }
   }
}

State *CustomizeState::change() {
   return new MenuState();
}
