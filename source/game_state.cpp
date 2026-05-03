#include "game_state.hpp"
#include "data.hpp"

GameState::GameState() {
   player.position = {1250, 1250};
   player.init({0, 0, 2500, 2500}, getCustomizationData());
   camera.init(&player, {0, 0, 2500, 2500}, player.position, 0.75f, 0.25f, 4.0f);
}

GameState::~GameState() {

}

void GameState::update() {
   camera.updateControls();
}

void GameState::render() {
   BeginMode2D(camera.camera);
      player.render();
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
