#include "game_state.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "render.hpp"
#include <raymath.h>

GameState::GameState() {
   setup(getLevel(0));
}

GameState::~GameState() {

}

void GameState::setup(const Level &level) {
   map.init(level, camera, player);
   calculateCameraBounds();
}

void GameState::calculateCameraBounds() {
   Camera2D cam = camera.camera;
   Vector2 pos = GetScreenToWorld2D({0.0f, 0.0f}, cam) / tileSize;
   Vector2 size = getScreenSize() / cam.zoom / tileSize;

   cameraBounds.x = fmax(0, int(pos.x));
   cameraBounds.y = fmax(0, int(pos.y));
   cameraBounds.width  = fmin(map.sizeX - 1, int(pos.x + size.x));
   cameraBounds.height = fmin(map.sizeY - 1, int(pos.y + size.y));
}

void GameState::update() {
   camera.updateControls();
}

void GameState::render() {
   // game world
   BeginMode2D(camera.camera);
      map.render(player, cameraBounds);
   EndMode2D();

   // ui
   drawTextCentered(getFont("slackey"), getScreenCenter(), TextFormat("%lu/%lu", map.collectedCoins, map.coinCount), 50.0f, WHITE);
}

void GameState::fixedUpdate() {
   player.update(map);
   camera.update();
   calculateCameraBounds();
}

void GameState::updateResponsiveness() {

}

State *GameState::change() {
   return nullptr;
}
