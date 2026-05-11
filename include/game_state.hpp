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

   enum class State {playing, paused, won};

   State state = State::playing;
   Player player;
   Rectangle cameraBounds;
   CameraAA3 camera;
   Map map;

   Button pauseButton, restartButton;
   Text *continueText, *restartText, *menuText, *wonNextText, *wonRestartText, *wonMenuText;
   Navigation pauseNavig, wonNavig;

   RenderTexture pausedTexture;
   TextureAA3 coinAnimation, timerAnimation;

   bool shouldRestart = false;
   bool shouldGoToMainMenu = false;
   bool shouldGoToNextLevel = false;
   float pausedTimer = 0.0f;
   int viewPortSizeShaderLocation = 0;
   int fadeShaderLocation = 0;
};
