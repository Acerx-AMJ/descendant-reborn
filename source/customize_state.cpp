#include "customize_state.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "input.hpp"
#include "menu_state.hpp"
#include <raymath.h>

constexpr Rectangle bounds = {0.0f, 0.0f, 2000.0f, 1250.0f};
constexpr size_t extraButtons = 10;
constexpr size_t entriesPerColumn = 15;
constexpr size_t entryColumns = 5;
constexpr size_t entriesPerPage = entriesPerColumn * entryColumns;

CustomizeState::CustomizeState() {
   Shader shader = getShader("twocolor");
   primaryShaderLocation = GetShaderLocation(shader, "primary");
   secondaryShaderLocation = GetShaderLocation(shader, "secondary");

   Font font = getFont("slackey");
   Texture buttonTexture = getTexture("button");

   backButton = Button::make(getTexture("back"), {70.0f, 70.0f}),
   skinTab = Button::make(buttonTexture, {300.0f, 100.0f}, font, "SKIN", 50.0f),
   primaryTab = Button::make(buttonTexture, {300.0f, 100.0f}, font, "PRIMARY", 50.0f),
   secondaryTab = Button::make(buttonTexture, {300.0f, 100.0f}, font, "SECONDARY", 50.0f),
   hiddenButton = Button::make(getTexture("hidden"), {70.0f, 70.0f}),
   visibleButton = Button::make(getTexture("visible"), {70.0f, 70.0f}),
   diceButton = Button::make(getTexture("dice"), {70.0f, 70.0f});
   shadowButton = Button::make({}, {70.0f, 70.0f});
   prevPageButton = Button::make(getTexture("pointer_left"), {40.0f, 40.0f});
   nextPageButton = Button::make(getTexture("pointer_right"), {40.0f, 40.0f});

   skinButtons.addElements({backButton->copy(), skinTab->copy(), primaryTab->copy(), secondaryTab->copy(),
      visibleButton->copy(), diceButton->copy(), shadowButton->copy(), prevPageButton, nextPageButton->copy()});
   colorButtons.addElements({backButton->copy(), skinTab->copy(), primaryTab->copy(), secondaryTab->copy(),
      visibleButton->copy(), diceButton->copy(), shadowButton->copy(), prevPageButton->copy(), nextPageButton});
   noTabButtons.addElements({backButton, skinTab, primaryTab, secondaryTab, visibleButton, diceButton, shadowButton});
   hiddenButtons.addElements({hiddenButton});

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

   player.position = getOrigin({bounds.width, bounds.height});
   player.init(bounds, getCustomizationData());

   shadowButton->texture = getTexture(player.shadowsEnabled ? "shadows_enabled" : "shadows_disabled");
   camera.init(&player, bounds, player.position, 0.75f, 0.25f, 4.0f);
}

CustomizeState::~CustomizeState() {
   saveCustomizationData({player.iconID, player.primaryColorID, player.secondaryColorID, player.shadowsEnabled});
}

void CustomizeState::update() {
   camera.updateControls();

   if (backButton->clicked || handleKeyPressWithSound(KEY_ESCAPE)) {
      // only on escape
      if (visible && !backButton->clicked && tab != Tab::none) {
         tab = Tab::none;
      }
      else {
         fadingOut = true;
      }
   }

   // Update corner buttons
   if (diceButton->clicked || handleKeyPressWithSound(KEY_R)) {
      player.iconID = rand() % getPlayerIconCount();
      player.primaryColorID = rand() % getPlayerColorCount();
      player.secondaryColorID = rand() % getPlayerColorCount();

      skinButtons.setIndex(extraButtons + player.iconID);
      colorButtons.setIndex(extraButtons + (tab == Tab::primary ? player.primaryColorID : player.secondaryColorID));
   }

   if (shadowButton->clicked || handleKeyPressWithSound(KEY_V)) {
      player.shadowsEnabled = !player.shadowsEnabled;
      shadowButton->texture = getTexture(player.shadowsEnabled ? "shadows_enabled" : "shadows_disabled");
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

   if (visibleButton->clicked || handleKeyPressWithSound(KEY_H)) {
      hiddenButtons.setIndex(1);
      visibleButton->texture = getTexture("hidden");
      visible = false;
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
      updateButtons(skinButtons, skinPage, player.iconID);
   }
   else if (tab != Tab::none) {
      size_t &ID = tab == Tab::primary ? player.primaryColorID : player.secondaryColorID;
      updateButtons(colorButtons, colorPage, ID);
   }
   else {
      noTabButtons.update();
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

   // Update pages

   if (tab != Tab::none) {
      Navigation &navig = (tab == Tab::skin ? skinButtons : colorButtons);

      if (prevPageButton->clicked && navig.index >= extraButtons) {
         if (navig.index - extraButtons < entriesPerPage) {
            navig.index = extraButtons;
         }
         else {
            navig.index -= entriesPerPage;
         }
      }

      if (nextPageButton->clicked && navig.index >= extraButtons) {
         navig.index = std::min(navig.elements.size() - 1, navig.index + entriesPerPage);
      }
   }

   if (skinButtons.anySelected() && skinButtons.index >= extraButtons) {
      skinPage = (skinButtons.index - extraButtons) / entriesPerPage;
   }

   if (colorButtons.anySelected() && colorButtons.index >= extraButtons) {
      colorPage = (colorButtons.index - extraButtons) / entriesPerPage;
   }
}

void CustomizeState::updateButtons(Navigation &navig, size_t page, size_t &ID) {
   navig.manualNavigationHandling = /* false;// */ navig.index >= extraButtons;
   
   size_t max = std::min(navig.elements.size(), extraButtons + entriesPerPage * (page + 1));
   float offsetX = GetScreenWidth() * page;

   for (size_t i = extraButtons + entriesPerPage * page; i < max; ++i) {
      navig.getElement(i)->position.x -= offsetX;
   }

   navig.update();

   size_t index = navig.index - extraButtons;
   if (navig.manualNavigationHandling) {
      // don't touch this masterpiece, it works
      if (navig.shouldGoLeft) {
         if (index % entriesPerColumn == 0 && index < entriesPerPage) {
            navig.index = extraButtons;
         }
         else if (index % entriesPerColumn == 0) {
            navig.index = navig.index - entriesPerPage + entriesPerColumn;
         }

         navig.index -= 1;
      }
      else if (navig.shouldGoRight) {
         if ((index + 1) % entriesPerColumn == 0) {
            navig.index = navig.index + entriesPerPage - entriesPerColumn;
         }
         navig.index = std::min(navig.index + 1, navig.elements.size() - 1);
      }
      else if (navig.shouldGoDown) {
         size_t row = (index % entriesPerPage) / entriesPerColumn;
         if (row == entryColumns - 1 || navig.index + entriesPerColumn >= navig.elements.size()) {
            navig.index = 5; // visible/hidden button
         }
         else {
            navig.index += entriesPerColumn;
         }
      }
      else if (navig.shouldGoUp) {
         size_t row = (index % entriesPerPage) / entriesPerColumn;
         if (row == 0) {
            navig.index = 1; // back button
         }
         else {
            navig.index -= entriesPerColumn;
         }
      }
   }

   for (size_t i = extraButtons + entriesPerPage * page; i < max; ++i) {
      TextureRect *rect = navig.getTextureRect(i);
      rect->position.x += offsetX;

      if (rect->clicked) {
         ID = rect->ID;
         navig.setIndex(i);
      }
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
   Font font = getFont("slackey");
   
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

   if (tab == Tab::skin && skinButtons.index >= extraButtons) {
      TextureRect *selected = skinButtons.getSelectedTextureRect();
      // 64/y = 68/x; x = 68y/64; x = 1.0625y
      // where 64 is the default texture size and 68 is the texture size of the button pointer,
      // and where y is the size of the button, and x is the size of the pointer
      float offsetX = GetScreenWidth() * skinPage;
      float scale = getCubicRatio() * selected->scale * 1.0625f;
      drawTextureCentered(smallPointerTexture, {selected->position.x - offsetX, selected->position.y}, Vector2Scale(selected->size, scale), WHITE);
   }

   if ((tab == Tab::primary || tab == Tab::secondary) && colorButtons.index >= extraButtons) {
      TextureRect *selected = colorButtons.getSelectedTextureRect();
      float offsetX = GetScreenWidth() * colorPage;
      float scale = getCubicRatio() * selected->scale * 1.0625f;
      drawTextureCentered(smallPointerTexture, {selected->position.x - offsetX, selected->position.y}, Vector2Scale(selected->size, scale), WHITE);
   }

   if (tab != Tab::none) {
      size_t page = (tab == Tab::skin ? skinPage : colorPage);
      size_t maxPages = ((tab == Tab::skin ? skinButtons : colorButtons).elements.size() - extraButtons) / entriesPerPage;
      float cr = getCubicRatio();

      drawTextCentered(font, {getScreenCenter().x, GetScreenHeight() - 55.0f * cr}, TextFormat("%lu/%lu", page + 1, maxPages + 1), 50.0f, WHITE);
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
   shadowButton->position = {cr * 235.0f, GetScreenHeight() - cr * 55.0f};

   prevPageButton->position = {getScreenCenter().x - 125.0f * cr, GetScreenHeight() - 55.0f * cr};
   nextPageButton->position = {getScreenCenter().x + 125.0f * cr, GetScreenHeight() - 55.0f * cr};

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
