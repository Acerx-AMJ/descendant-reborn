#include "game_state.hpp"
#include "data.hpp"

GameState::GameState() {
   setup(getLevel(0));
}

GameState::~GameState() {

}

void GameState::setup(const Level &level) {
   map.init(level, camera, player);
}

void GameState::update() {
   camera.updateControls();
}

void GameState::render() {
   BeginMode2D(camera.camera);
      map.render(player);
   EndMode2D();
}

void GameState::fixedUpdate() {
   player.update();
   camera.update();
}

void GameState::updateResponsiveness() {

}

State *GameState::change() {
   return nullptr;
}
