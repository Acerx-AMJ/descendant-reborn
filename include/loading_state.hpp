#pragma once
#include "state.hpp"
#include <string>

struct LoadingState: public State {
   LoadingState();
   ~LoadingState() = default;

   void update() override;
   void render() override;
   void fixedUpdate() override;
   void updateResponsiveness() override;
   State *change() override;

private:

   enum class Load {fonts, textures, shaders, sounds, count};

   std::string loadingText, splash;
   Load phase = Load::fonts;

   float finalWaitTimer = 0.0f;
   float iconRotation = 0.0f;
};
