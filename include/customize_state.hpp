#pragma once
#include "camera.hpp"
#include "navig.hpp"
#include "player.hpp"
#include "state.hpp"

struct CustomizeState: public State {
   CustomizeState();
   ~CustomizeState();

   void update() override;
   void render() override;
   void fixedUpdate() override;
   void updateResponsiveness() override;
   State *change() override;

   void updateButtons(Navigation &navig, size_t page, size_t &ID);

private:

   enum class Tab {none, skin, primary, secondary};
   Tab tab = Tab::none;

   CameraDR3 camera;
   Player player;
   Navigation skinButtons, colorButtons, hiddenButtons, noTabButtons;
   Button *backButton, *skinTab, *primaryTab, *secondaryTab, *hiddenButton, *visibleButton, *diceButton,
      *shadowButton, *prevPageButton, *nextPageButton;
   size_t skinPage = 0;
   size_t colorPage = 0;

   int primaryShaderLocation = 0;
   int secondaryShaderLocation = 0;
   bool visible = true;
};
