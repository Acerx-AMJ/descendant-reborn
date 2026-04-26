#pragma once
#include "navig.hpp"
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

   Navigation skinButtons, colorButtons, hiddenButtons, noTabButtons;
   Button *backButton, *skinTab, *primaryTab, *secondaryTab, *hiddenButton, *visibleButton, *diceButton;

   size_t skinID = 0;
   size_t primaryColorID = 0;
   size_t secondaryColorID = 0;

   int primaryShaderLocation = 0;
   int secondaryShaderLocation = 0;
   bool visible = true;
};
