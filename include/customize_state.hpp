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

private:

   enum class Tab {none, skin, primary, secondary};
   Tab tab = Tab::none;

   CameraDR3 camera;
   Player player;
   Navigation skinButtons, colorButtons, hiddenButtons, noTabButtons;
   Button *backButton, *skinTab, *primaryTab, *secondaryTab, *hiddenButton, *visibleButton, *diceButton;

   int primaryShaderLocation = 0;
   int secondaryShaderLocation = 0;
   bool visible = true;
};
