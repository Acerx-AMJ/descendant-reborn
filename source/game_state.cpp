#include "game_state.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "input.hpp"
#include "menu_state.hpp"
#include "render.hpp"
#include <raymath.h>

GameState::GameState() {
   pauseButton.init(getTexture("pause"), {70.0f, 70.0f});
   restartButton.init(getTexture("restart"), {70.0f, 70.0f});

   Font font = getFont("slackey");
   continueText = Text::make(font, "CONTINUE", 50.0f);
   restartText = Text::make(font, "RESTART", 50.0f);
   menuText = Text::make(font, "MAIN MENU", 50.0f);
   pauseNavig.addElements({continueText, restartText, menuText});

   Shader shader = getShader("blur");
   viewPortSizeShaderLocation = GetShaderLocation(shader, "viewport");
   fadeShaderLocation = GetShaderLocation(shader, "fade");

   pausedTexture.id = 0;
   updateResponsiveness();

   setup(getLevel(0));
}

GameState::~GameState() {
   if (pausedTexture.id != 0) {
      UnloadRenderTexture(pausedTexture);
   }
}

void GameState::setup(const Level &level) {
   map.init(level, camera, player);
   calculateCameraBounds();

   initAnimationIfExists(coinAnimation, level.coinTile);
   initAnimationIfExists(timerAnimation, "timer"); // TODO: add to config
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
   pauseButton.update();
   restartButton.update();

   if (pauseButton.clicked || handleKeyPressWithSound(KEY_ESCAPE)) {
      state = (state == State::paused ? State::playing : State::paused);
      pauseNavig.resetIndex();
   }

   if (restartButton.clicked || handleKeyPressWithSound(KEY_R)) {
      state = State::restarting;
      fadingOut = true;
   }

   if (state == State::paused) {
      pausedTimer = fmin(1.0f, pausedTimer + GetFrameTime() * 2.0f);
      pauseNavig.update();

      if (continueText->clicked) {
         state = State::playing;
      }

      if (restartText->clicked) {
         state = State::restarting;
         fadingOut = true;
      }

      if (menuText->clicked) {
         state = State::menu;
         fadingOut = true;
      }
      return;
   }

   pausedTimer = fmax(0.0f, pausedTimer - GetFrameTime() * 2.0f);
   camera.updateControls();
}

float fadeBlur(float t) {
    t = fmax(0.0f, fmin(1.0f, t));
    return t * t * (3.0f - 2.0f * t);
}

void GameState::render() {
   // game world
   if (pausedTimer != 0.0f) {
      Shader shader = getShader("blur");
      Vector2 size = getScreenSize();
      float fade = fadeBlur(pausedTimer);

      SetShaderValue(shader, viewPortSizeShaderLocation, &size, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, fadeShaderLocation, &fade, SHADER_UNIFORM_FLOAT);

      BeginTextureMode(pausedTexture);
      ClearBackground(BLACK);
      BeginMode2D(camera.camera);
         map.render(player, cameraBounds);
      EndMode2D();
      EndTextureMode();

      BeginShaderMode(shader);
         DrawTextureRec(pausedTexture.texture, {0, 0, (float)pausedTexture.texture.width, (float)-pausedTexture.texture.height}, {0.0f, 0.0f}, WHITE);
      EndShaderMode();
   }
   else {
      BeginMode2D(camera.camera);
         map.render(player, cameraBounds);
      EndMode2D();
   }

   // ui
   Font font = getFont("slackey");
   float cr = getCubicRatio();

   if (state == State::paused) {
      DrawRectangleV({0.0f, 0.0f}, {500.0f * cr, GetScreenHeight() * 1.0f}, Fade(BLACK, 0.5f));
      drawTextSemiCentered(font, {cr * 100.0f, cr * 100.0f}, map.name.c_str(), 50.0f, WHITE);
      drawTextSemiCentered(font, {cr * 100.0f, cr * 150.0f}, map.chapter.c_str(), 35.0f, {200, 200, 200, 255});
      pauseNavig.render();

      if (pauseNavig.anySelected()) {
         drawTextureCentered(getTexture("lotus"), {cr * 50.0f, pauseNavig.getSelectedElement()->position.y}, cubicSize(65.0f + 10.0f * sin(GetTime() * 3.0f)), WHITE, GetTime() * 40.0f);
      }
   }
   else {
      pauseButton.render();
      restartButton.render();
   }

   float down = GetScreenHeight() - 50.0f * cr;

   drawTextureAnimatedCentered(coinAnimation, {cr * 720.0f, down}, cubicSize(50.0f), WHITE);
   drawTextSemiCentered(font, {cr * 760.0f, down}, TextFormat("%lu/%lu", map.collectedCoins, map.coinCount), 35.0f, WHITE);

   drawTextureAnimatedCentered(timerAnimation, {cr * 1300.0f, down}, cubicSize(50.0f), WHITE);
   drawTextSemiCentered(font, {cr * 1340.0f, down}, TextFormat("temp text."), 35.0f, WHITE);
}

void GameState::fixedUpdate() {
   if (state != State::paused) {
      player.update(map);
   }
   camera.update();
   calculateCameraBounds();
}

void GameState::updateResponsiveness() {
   float cr = getCubicRatio();
   pauseButton.position = {cr * 55.0f, GetScreenHeight() - cr * 55.0f};
   restartButton.position = {cr * 145.0f, GetScreenHeight() - cr * 55.0f};

   continueText->position = {cr * 100.0f, cr * 300.0f};
   restartText->position = {cr * 100.0f, cr * 400.0f};
   menuText->position = {cr * 100.0f, cr * 500.0f};

   if (pausedTexture.id != 0) {
      UnloadRenderTexture(pausedTexture);
   }
   pausedTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
}

State *GameState::change() {
   if (state == State::restarting) {
      return new GameState();
   }

   if (state == State::menu) {
      return new MenuState();
   }
   return nullptr;
}
