#pragma once
#include "camera.hpp"
#include "state.hpp"

struct GameState: public State {
   GameState();
   ~GameState();

   void update() override;
   void render() override;
   void fixedUpdate() override;
   void updateResponsiveness() override;
   State *change() override;

private:

   Player player;
   CameraDR3 camera;
};
