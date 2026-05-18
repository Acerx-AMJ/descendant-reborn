#include "customize_state.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "input.hpp"
#include "math.hpp"
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

   backButton = Button::make(getTexture("back"), {70.0f, 70.0f});
   skinTab = Button::make(buttonTexture, {300.0f, 100.0f}, font, "SKIN", 50.0f);
   primaryTab = Button::make(buttonTexture, {300.0f, 100.0f}, font, "PRIMARY", 50.0f);
   secondaryTab = Button::make(buttonTexture, {300.0f, 100.0f}, font, "SECONDARY", 50.0f);
   hiddenButton = Button::make(getTexture("hidden"), {70.0f, 70.0f});
   visibleButton = Button::make(getTexture("visible"), {70.0f, 70.0f});
   diceButton = Button::make(getTexture("dice"), {70.0f, 70.0f});
   shadowButton = Button::make({}, {70.0f, 70.0f});
   prevPageButton = Button::make(getTexture("pointer_left"), {40.0f, 40.0f});
   nextPageButton = Button::make(getTexture("pointer_right"), {40.0f, 40.0f});

   // skin buttons
   // up, left, right, down
   float bottomLeftIcons = fmin(extraButtons + getPlayerIconCount(), extraButtons + (entriesPerColumn * (entryColumns - 1)));

   /* 1*/ skinButtons.addElement(backButton, 0, 0, 2, extraButtons);
   /* 2*/ skinButtons.addElement(skinTab, 0, 1, 3, extraButtons);
   /* 3*/ skinButtons.addElement(primaryTab, 0, 2, 4, extraButtons);
   /* 4*/ skinButtons.addElement(secondaryTab, 0, 3, 0, extraButtons);
   /* 5*/ skinButtons.addElement(visibleButton, bottomLeftIcons, 0, 6, 0);
   /* 6*/ skinButtons.addElement(diceButton, bottomLeftIcons, 5, 7, 0);
   /* 7*/ skinButtons.addElement(shadowButton, bottomLeftIcons, 6, 8, 0);
   /* 8*/ skinButtons.addElement(prevPageButton, bottomLeftIcons, 7, 9, 0);
   /* 9*/ skinButtons.addElement(nextPageButton, bottomLeftIcons, 8, 0, 0);
   skinButtons.setNoSelectionNavigDefault(1);

   // color buttons
   float bottomLeftColors = fmin(extraButtons + getPlayerColorCount(), extraButtons + (entriesPerColumn * (entryColumns - 1)));

   /* 1*/ colorButtons.addElement(backButton, 0, 0, 2, extraButtons);
   /* 2*/ colorButtons.addElement(skinTab, 0, 1, 3, extraButtons);
   /* 3*/ colorButtons.addElement(primaryTab, 0, 2, 4, extraButtons);
   /* 4*/ colorButtons.addElement(secondaryTab, 0, 3, 0, extraButtons);
   /* 5*/ colorButtons.addElement(visibleButton, bottomLeftColors, 0, 6, 0);
   /* 6*/ colorButtons.addElement(diceButton, bottomLeftColors, 5, 7, 0);
   /* 7*/ colorButtons.addElement(shadowButton, bottomLeftColors, 6, 8, 0);
   /* 8*/ colorButtons.addElement(prevPageButton, bottomLeftColors, 7, 9, 0);
   /* 9*/ colorButtons.addElement(nextPageButton, bottomLeftColors, 8, 0, 0);
   colorButtons.setNoSelectionNavigDefault(1);

   // no tab buttons
   /* 1*/ noTabButtons.addElement(backButton, 0, 0, 2, 5);
   /* 2*/ noTabButtons.addElement(skinTab, 0, 1, 3, 5);
   /* 3*/ noTabButtons.addElement(primaryTab, 0, 2, 4, 5);
   /* 4*/ noTabButtons.addElement(secondaryTab, 0, 3, 0, 5);
   /* 5*/ noTabButtons.addElement(visibleButton, 1, 0, 6, 0);
   /* 6*/ noTabButtons.addElement(diceButton, 1, 5, 7, 0);
   /* 7*/ noTabButtons.addElement(shadowButton, 1, 6, 0, 0);
   noTabButtons.setNoSelectionNavigDefault(1);

   // hidden buttons
   /* 1*/ hiddenButtons.addElement(hiddenButton, 0, 0, 0, 0);
   hiddenButtons.setNoSelectionNavigDefault(1);

   std::vector<std::string> &iconContainer = getPlayerIconContainer();
   for (size_t i = 0; i < iconContainer.size(); ++i) {
      TextureRect *rect = TextureRect::make(getTexture(iconContainer[i]), {70.0f, 70.0f});
      size_t inav = i + extraButtons;
      size_t row = (i % entriesPerPage) / entriesPerColumn;
      size_t up = (row == 0 ? 1 : inav - entriesPerColumn);

      size_t left = inav;
      if (i % entriesPerColumn == 0 && i < entriesPerPage) {
         left = extraButtons + 1;
      }
      else if (i % entriesPerColumn == 0) {
         left = inav - entriesPerPage + entriesPerColumn;
      }
      left -= 1;
   
      size_t right = inav;
      if ((i + 1) % entriesPerColumn == 0) {
         right = inav + entriesPerPage - entriesPerColumn;
      }
      right = std::min(right + 1, extraButtons + iconContainer.size() - 1);
      
      size_t down = inav;
      if (row == entryColumns - 1 || i + entriesPerColumn >= iconContainer.size()) {
         down = 5;
      }
      else {
         down = inav + entriesPerColumn;
      }
      skinButtons.addElement(rect, up, left, right, down);
   }

   std::vector<Vector3> &colorContainer = getPlayerColorContainer();
   for (size_t i = 0; i < colorContainer.size(); ++i) {
      Vector3 translated = colorContainer[i] * 255.0f;
      Color final = C4(translated.x, translated.y, translated.z, 255);
      TextureRect *rect = TextureRect::make(final, {70.0f, 70.0f});
      size_t inav = i + extraButtons;
      size_t row = (i % entriesPerPage) / entriesPerColumn;
      size_t up = (row == 0 ? 1 : inav - entriesPerColumn);

      size_t left = inav;
      if (i % entriesPerColumn == 0 && i < entriesPerPage) {
         left = extraButtons + 1;
      }
      else if (i % entriesPerColumn == 0) {
         left = inav - entriesPerPage + entriesPerColumn;
      }
      left -= 1;
   
      size_t right = inav;
      if ((i + 1) % entriesPerColumn == 0) {
         right = inav + entriesPerPage - entriesPerColumn;
      }
      right = std::min(right + 1, extraButtons + colorContainer.size() - 1);
      
      size_t down = inav;
      if (row == entryColumns - 1 || i + entriesPerColumn >= colorContainer.size()) {
         down = 5;
      }
      else {
         down = inav + entriesPerColumn;
      }
      colorButtons.addElement(rect, up, left, right, down);
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

      skinButtons.index = extraButtons + player.iconID;
      colorButtons.index = extraButtons + (tab == Tab::primary ? player.primaryColorID : player.secondaryColorID);
   }

   if (shadowButton->clicked || handleKeyPressWithSound(KEY_V)) {
      player.shadowsEnabled = !player.shadowsEnabled;
      shadowButton->texture = getTexture(player.shadowsEnabled ? "shadows_enabled" : "shadows_disabled");
   }

   if (!visible) {
      hiddenButtons.update();

      if (hiddenButton->clicked || handleKeyPressWithSound(KEY_H)) {
         // 5 is visible button
         skinButtons.index = 5;
         colorButtons.index = 5;
         noTabButtons.index = 5;
         visibleButton->texture = getTexture("visible");
         visible = true;
      }
      return;
   }

   if (visibleButton->clicked || handleKeyPressWithSound(KEY_H)) {
      hiddenButtons.index = 1;
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
      skinButtons.index = extraButtons + player.iconID;
   }

   if (lastTab != tab && tab == Tab::primary) {
      colorButtons.index = extraButtons + player.primaryColorID;
   }

   if (lastTab != tab && tab == Tab::secondary) {
      colorButtons.index = extraButtons + player.secondaryColorID;
   }

   // Update pages

   if (tab != Tab::none) {
      Navigation &navig = (tab == Tab::skin ? skinButtons : colorButtons);

      if ((prevPageButton->clicked || handleKeyPressWithSound(KEY_COMMA)) && navig.index >= extraButtons) {
         if (navig.index - extraButtons < entriesPerPage) {
            navig.index = extraButtons;
         }
         else {
            navig.index -= entriesPerPage;
         }
      }

      if ((nextPageButton->clicked || handleKeyPressWithSound(KEY_PERIOD)) && navig.index >= extraButtons) {
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
   size_t max = std::min(navig.elements.size(), extraButtons + entriesPerPage * (page + 1));
   float offsetX = GetScreenWidth() * page;

   for (size_t i = extraButtons + entriesPerPage * page; i < max; ++i) {
      navig.getElement(i)->position.x -= offsetX;
   }

   navig.update();

   for (size_t i = extraButtons + entriesPerPage * page; i < max; ++i) {
      TextureRect *rect = navig.get<TextureRect>(i);
      rect->position.x += offsetX;

      if (rect->clicked) {
         ID = rect->ID;
         navig.index = i;
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
      TextureRect *selected = skinButtons.getSelected<TextureRect>();
      float offsetX = GetScreenWidth() * skinPage;
      float scale = getCubicRatio() * selected->scale * 1.0625f;
      drawTextureCentered(smallPointerTexture, {selected->position.x - offsetX, selected->position.y}, Vector2Scale(selected->size, scale), WHITE);
   }

   if ((tab == Tab::primary || tab == Tab::secondary) && colorButtons.index >= extraButtons) {
      TextureRect *selected = colorButtons.getSelected<TextureRect>();
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
   if (!fadingOut) {
      player.update();
      camera.update();
   }
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

   float topOffset = (primaryTab->position.y + primaryTab->size.y / 2.0f) + 100.0f * cr;
   float workingHeight = (visibleButton->position.y - visibleButton->size.y / 2.0f) - topOffset;
   float workingWidth = workingHeight * getAspectRatio();
   float minimumWorkingSize = fmax(workingHeight, workingWidth);

   float singleEntrySizeWithPadding = minimumWorkingSize / entriesPerColumn;
   float singleEntrySize = singleEntrySizeWithPadding * (6.0f / 7.0f);
   float singleEntrySizeTranslated = singleEntrySize / cr;

   float startX = fmax(singleEntrySize / 2.0f, singleEntrySize / 2.0f + (GetScreenWidth() - minimumWorkingSize) / 2.0f);
   float startY = singleEntrySize / 2.0f + topOffset;

   float positionX = startX;
   float positionY = startY;

   for (size_t i = extraButtons; i < skinButtons.elements.size(); ++i) {
      TextureRect *button = skinButtons.get<TextureRect>(i);
      button->ID = i - extraButtons;
      button->position = {positionX, positionY};
      button->size = {singleEntrySizeTranslated, singleEntrySizeTranslated};

      positionX += singleEntrySizeWithPadding;
      size_t index = i - extraButtons + 1;
      
      if (index % entriesPerColumn == 0) {
         float screens = GetScreenWidth() * size_t(index / entriesPerPage);
         positionX = startX + screens;
         positionY += singleEntrySizeWithPadding;
      }

      if (index % entriesPerPage == 0) {
         float screens = GetScreenWidth() * size_t(index / entriesPerPage);
         positionX = startX + screens;
         positionY = startY;
      }
   }

   positionX = startX;
   positionY = startY;

   for (size_t i = extraButtons; i < colorButtons.elements.size(); ++i) {
      TextureRect *button = colorButtons.get<TextureRect>(i);
      button->ID = i - extraButtons;
      button->position = {positionX, positionY};
      button->size = {singleEntrySizeTranslated, singleEntrySizeTranslated};

      positionX += singleEntrySizeWithPadding;
      size_t index = i - extraButtons + 1;

      if (index % entriesPerColumn == 0) {
         float screens = GetScreenWidth() * size_t(index / entriesPerPage);
         positionX = startX + screens;
         positionY += singleEntrySizeWithPadding;
      }

      if (index % entriesPerPage == 0) {
         float screens = GetScreenWidth() * size_t(index / entriesPerPage);
         positionX = startX + screens;
         positionY = startY;
      }
   }
}

State *CustomizeState::change() {
   return new MenuState();
}
