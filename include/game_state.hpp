#pragma once
#include "camera.hpp"
#include "map.hpp"
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

   Player player;
   Rectangle cameraBounds;
   CameraDR3 camera;
   Map map;
};
