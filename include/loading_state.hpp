#pragma once
#include "state.hpp"
#include <string>

struct LoadingState: public State {
   LoadingState();
   ~LoadingState() = default;

   State *change() override;

   void update() override;
   void render() override;

private:

   enum class Load {fonts, textures, shaders, sounds, data, count};

   std::string loadingText, splash;
   Load phase = Load::fonts;

   float finalWaitTimer = 0.0f;
   float iconRotation = 0.0f;
};
