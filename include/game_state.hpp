#pragma once
#include "camera.hpp"
#include "map.hpp"
#include "navig.hpp"
#include "state.hpp"

struct GameState: public State {
   GameState();
   ~GameState();

   void setup(const Level &level);
   void calculateCameraBounds();

   void update() override;
   void render() override;
   void fixedUpdate() override;
   void updateResponsiveness() override;
   State *change() override;

private:

   enum class State {playing, paused, restarting, menu};

   State state = State::playing;
   Player player;
   Rectangle cameraBounds;
   CameraDR3 camera;
   Map map;

   Button pauseButton, restartButton;
   Text *continueText, *restartText, *menuText;
   Navigation pauseNavig;

   RenderTexture pausedTexture;
   float pixelScale = 0.0f;
   int viewPortSizeShaderLocation = 0;
   int pixelSizeShaderLocation = 0;
   int fadeShaderLocation = 0;
   int timeShaderLocation = 0;
};
