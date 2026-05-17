#include "game_state.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "input.hpp"
#include "math.hpp"
#include "menu_state.hpp"
#include "particles.hpp"
#include "render.hpp"
#include "sound.hpp"
#include <raymath.h>

GameState::GameState() {
   pauseButton.init(getTexture("pause"), {70.0f, 70.0f});
   restartButton.init(getTexture("restart"), {70.0f, 70.0f});

   Font font = getFont("slackey");
   continueText = Text::make(font, "CONTINUE", 50.0f);
   restartText = Text::make(font, "RESTART", 50.0f);
   menuText = Text::make(font, "MAIN MENU", 50.0f);
   pauseNavig.addElements({continueText, restartText, menuText});

   wonNextText = Text::make(font, "NEXT", 50.0f);
   wonRestartText = Text::make(font, "RESTART", 50.0f);
   wonMenuText = Text::make(font, "MAIN MENU", 50.0f);
   wonNavig.addElements({wonNextText, wonRestartText, wonMenuText});

   Shader blurShader = getShader("blur");
   viewPortSizeShaderLocation = GetShaderLocation(blurShader, "viewport");
   fadeShaderLocation = GetShaderLocation(blurShader, "fade");

   cameraUI.init(nullptr, getRectangle({0, 0}, getScreenSize()), getScreenCenter(), 1.0f, 1.0f, 1.0f);
   cameraUI.update();

   resultColorScheme = getDefaultResultColorScheme();
   resultText = getRandomDefaultResultLine();
   pausedTexture.id = 0;
   updateResponsiveness();
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
   initAnimationIfExists(timerAnimation, level.timerTile);
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
   switch (state) {
   case State::playing: updatePlayingState(); break;
   case State::paused:  updatePausedState();  break;
   case State::won:     updateWonState();     break;
   }
}

void GameState::render() {
   Shader shader = getShader("blur");
   Vector2 size = getScreenSize();
   float fade = pausedTimer * pausedTimer * (3.0f - 2.0f * pausedTimer);
   bool paused = state != State::playing;

   SetShaderValue(shader, viewPortSizeShaderLocation, &size, SHADER_UNIFORM_VEC2);
   SetShaderValue(shader, fadeShaderLocation, &fade, SHADER_UNIFORM_FLOAT);

   BeginTextureMode(pausedTexture);
   ClearBackground(BLACK);
   BeginMode2D(camera.camera);
      map.render(player, cameraBounds, paused);
   EndMode2D();
   EndTextureMode();

   BeginShaderMode(shader);
      DrawTextureRec(pausedTexture.texture, {0, 0, (float)pausedTexture.texture.width, (float)-pausedTexture.texture.height}, {0.0f, 0.0f}, WHITE);
   EndShaderMode();

   BeginMode2D(cameraUI.camera);
      switch (state) {
      case State::playing: renderPlayingState(); break;
      case State::paused:  renderPausedState();  break;
      case State::won:     renderWonState();     break;
      }

      if (state == State::won) {
         return;
      }
      Font font = getFont("slackey");
      float cr = getCubicRatio();
      float down = GetScreenHeight() - 50.0f * cr;

      drawTextureAnimatedCentered(coinAnimation, {cr * 720.0f, down}, cubicSize(50.0f), WHITE, paused);
      drawTextSemiCentered(font, {cr * 760.0f, down}, TextFormat("%lu/%lu", map.collectedCoins, map.coinCount), 35.0f, WHITE);

      drawTextureAnimatedCentered(timerAnimation, {cr * 1300.0f, down}, cubicSize(50.0f), WHITE, paused || !startCountingTime);
      drawTextSemiCentered(font, {cr * 1340.0f, down}, (startCountingTime ? TextFormat("%05.2f", gameTime) : "--.--"), 35.0f, WHITE);
   EndMode2D();
}

void GameState::fixedUpdate() {
   if (state != State::paused && state != State::won) {
      player.update(map);

      if (!startCountingTime && (player.direction.x != 0.0f || player.direction.y != 0.0f)) {
         playSound("horn");
         startCountingTime = true;
      }

      if (player.finished) {
         playSound("win");
         pausedTimer = 0.0f;
         state = State::won;
         camera.shake(30.0f, 0.5f);
         cameraUI.shake(30.0f, 0.5f);

         starCount += (map.coinCount <= map.collectedCoins);
         starCount += (map.coinCount <= map.collectedCoins * 2);
         starCount += (gameTime <= map.time);
         previousTime = getLevelData(map.levelID).time;
         saveLevelDataOnNewScore({map.coinCount <= map.collectedCoins, gameTime <= map.perfectTime
            && starCount == 3, gameTime, camera.camera.zoom, starCount}, map.coinCount <= map.collectedCoins,
            map.levelID);
         wonNextText->disabled = (!getLevelData(map.levelID).completed && map.levelID != getLevelCount() - 1);
      }
   }
   cameraUI.update();
   camera.update();
   calculateCameraBounds();
}

void GameState::updateResponsiveness() {
   float cr = getCubicRatio();
   pauseButton.position = {cr * 55.0f, GetScreenHeight() - cr * 55.0f};
   restartButton.position = {cr * 145.0f, GetScreenHeight() - cr * 55.0f};

   continueText->position = wonNextText->position    = {cr * 100.0f, cr * 300.0f};
   restartText->position  = wonRestartText->position = {cr * 100.0f, cr * 400.0f};
   menuText->position     = wonMenuText->position    = {cr * 100.0f, cr * 500.0f};

   if (pausedTexture.id != 0) {
      UnloadRenderTexture(pausedTexture);
   }
   pausedTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
}

State *GameState::change() {
   saveLevelZoom(map.levelID, camera.camera.zoom);

   if (shouldRestart) {
      GameState *game = new GameState();
      game->setup(getLevel(map.levelID));
      return game;
   }

   if (shouldGoToMainMenu) {
      return new MenuState();
   }

   if (shouldGoToNextLevel) {
      GameState *game = new GameState();
      game->setup(getLevel((map.levelID + 1) % getLevelCount()));
      return game;
   }
   return nullptr;
}

void GameState::updatePlayingState() {
   pauseButton.update();
   restartButton.update();

   if (pauseButton.clicked || handleKeyPressWithSound(KEY_ESCAPE)) {
      state = State::paused;
      pauseNavig.resetIndex();
   }

   if (restartButton.clicked || handleKeyPressWithSound(KEY_R)) {
      shouldRestart = true;
      fadingOut = true;
   }

   if (startCountingTime) {
      gameTime += GetFrameTime();
   }

   pausedTimer = fmax(0.0f, pausedTimer - GetFrameTime() * 2.0f);
   camera.updateControls();
}

void GameState::updatePausedState() {
   pausedTimer = fmin(1.0f, pausedTimer + GetFrameTime() * 2.0f);
   pauseNavig.update();

   if (continueText->clicked || handleKeyPressWithSound(KEY_ESCAPE)) {
      state = State::playing;
   }

   if (restartText->clicked || handleKeyPressWithSound(KEY_R)) {
      shouldRestart = true;
      fadingOut = true;
   }

   if (menuText->clicked) {
      shouldGoToMainMenu = true;
      fadingOut = true;
   }
}

void GameState::updateWonState() {
   pausedTimer = fmin(1.0f, pausedTimer + GetFrameTime());
   wonNavig.update();

   if (wonNextText->clicked) {
      shouldGoToNextLevel = true;
      fadingOut = true;
      return;
   }

   if (wonRestartText->clicked || handleKeyPressWithSound(KEY_R)) {
      shouldRestart = true;
      fadingOut = true;
      return;
   }

   if (wonMenuText->clicked) {
      shouldGoToMainMenu = true;
      fadingOut = true;
      return;
   }

   // Fill up the stars
   if (starsFilled < 3) {
      starTimer += GetFrameTime();

      float &scale = starScales[starsFilled];
      if (starTimer >= 0.55f && scale == 1.0f) {
         scale = 1.66f;

         bool isAStar = starsFilled < starCount;
         bool isASilverStar = !isAStar && starsFilled < getLevelData(map.levelID).stars;
         starStatus[starsFilled] = (isASilverStar ? 2 : isAStar);
         playSound(isAStar ? "hit" : "hit_small");

         if (starsFilled == 2) {
            if (isAStar) {
               camera.shake(35.0f, 0.6f);
               cameraUI.shake(35.0f, 0.6f);
               spawnStarParticles(V2(1600.0f, 300.0f) * getCubicRatio());
            }

            // data.cpp:loadResults for explanation
            size_t finalResult = 0;
            if (previousTime != std::numeric_limits<float>::max() && previousTime > gameTime && map.coinCount <= map.collectedCoins) {
               finalResult = 5;

               camera.shake(35.0f, 0.6f);
               cameraUI.shake(35.0f, 0.6f);
               playSound("win");
               spawnConfetti();
            }
            else if (gameTime <= map.perfectTime && starCount == 3) {
               finalResult = 4;
               playSound("win");
            }
            else {
               finalResult = starCount;
               if (finalResult == 0) {
                  playSound("horrible");
               }
            }

            resultColorScheme = getResultColorSchemeBasedOnPerformance(finalResult);
            resultText = getRandomResultLineBasedOnPerformance(finalResult);
            resultScale = 1.66f;
         }
      }

      if (starTimer >= 0.55f) {
         scale *= 1.0f - GetFrameTime() * 2.0f;
         if (scale <= 1.0f) {
            scale = 1.0f;
            starTimer -= 0.55f;
            starsFilled += 1;
         }
      }
   }

   if (starTimer >= 0.55f) {
      resultScale = fmax(1.0f, resultScale * (1.0f - GetFrameTime() * 2.0f));
   }
}

void GameState::renderPlayingState() {
   pauseButton.render();
   restartButton.render();
}

void GameState::renderPausedState() {
   Font font = getFont("slackey");
   float cr = getCubicRatio();

   DrawRectangleV({0.0f, 0.0f}, {500.0f * cr, GetScreenHeight() * 1.0f}, Fade(BLACK, 0.5f));
   drawTextSemiCentered(font, {cr * 100.0f, cr * 100.0f}, map.name.c_str(), 50.0f, WHITE);
   drawTextSemiCentered(font, {cr * 100.0f, cr * 150.0f}, map.chapter.c_str(), 35.0f, {200, 200, 200, 255});
   pauseNavig.render();

   if (pauseNavig.anySelected()) {
      drawTextureCentered(getTexture("lotus"), {cr * 50.0f, pauseNavig.getSelectedElement()->position.y}, cubicSize(65.0f + 10.0f * sin(GetTime() * 3.0f)), WHITE, GetTime() * 40.0f);
   }
}

void GameState::renderWonState() {
   Font font = getFont("slackey");
   float cr = getCubicRatio();

   DrawRectangleV({0.0f, 0.0f}, getScreenSize(), Fade(BLACK, pausedTimer / 2.0f));
   DrawRectangleV({0.0f, 0.0f}, {500.0f * cr, GetScreenHeight() * 1.0f}, Fade(BLACK, 0.5f));
   drawTextSemiCentered(font, {cr * 100.0f, cr * 100.0f}, "Level Beat!", 50.0f, WHITE);
   drawTextSemiCentered(font, {cr * 100.0f, cr * 150.0f}, ("Next: " + getLevel((map.levelID + 1) % getLevelCount()).name).c_str(), 35.0f, {200, 200, 200, 255});
   wonNavig.render();

   if (wonNavig.anySelected() && (!wonNextText->disabled || wonNavig.getSelectedText() != wonNextText)) {
      drawTextureCentered(getTexture("lotus"), {cr * 50.0f, wonNavig.getSelectedElement()->position.y}, cubicSize(65.0f + 10.0f * sin(GetTime() * 3.0f)), WHITE, GetTime() * 40.0f);
   }

   if (wonNextText->disabled) {
      drawTextureCentered(getTexture("lock"), {cr * 50.0f, wonNextText->position.y}, cubicSize(65.0f + 10.0f * sin(GetTime() * 3.0f)), WHITE);

      if (wonNextText->hovering) {
         Vector2 position = (wonNavig.getSelectedText() == wonNextText ? wonNextText->position : GetMousePosition());
         Vector2 size = getTextSize(font, "You must collect all coins\n to unlock the next level!", getFontSize(40.0f), getFontSize(1.0f));
         DrawRectanglePro(getRectangle(position, size), {0.0f, size.y / 2.0f}, 0.0f, Fade(BLACK, 0.75f));
         drawTextSemiCentered(font, position, "You must collect all coins\n to unlock the next level!", 40.0f, WHITE);
      }
   }

   // draw the stars
   for (int i = 0; i < 3; ++i) {
      Texture texture = getTexture(starStatus[i] == 2 ? "silver_star" : (starStatus[i] == 1 ? "star" : "unknown"));
      Vector2 position = V2(1200.0f, 300.0f) * cr; // star 1
      float rotation = -22.5f;
      
      if (i == 1) {
         position = V2(1400.0f, 250.0f) * cr; // star 2
         rotation = 0.0f;
      }
      else if (i == 2) {
         position = V2(1600.0f, 300.0f) * cr; // star 3
         rotation = 22.5f;
      }
      drawTextureCentered(texture, position, V2(110.0f) * cr * starScales[i], WHITE, rotation);
   }
   renderParticles(getStarParticleCluster());

   resultColorFade += GetFrameTime() * 3.0f;
   if (resultColorFade >= 1.0f) {
      resultColorFade -= 1.0f;
      resultColorIndex = (resultColorIndex + 1) % resultColorScheme.size();
   }

   Color color = ColorLerp(
      resultColorScheme[resultColorIndex],
      resultColorScheme[(resultColorIndex + 1) % resultColorScheme.size()],
      resultColorFade
   );

   drawTextCentered(font, V2(1550.0f, 425.0f) * cr, resultText.c_str(), 70.0f * resultScale, color, 12.5f);
   if (map.coinCount <= map.collectedCoins) {
      drawTextCentered(font, V2(1400.0f, 500.0f) * cr, TextFormat("%05.2f", gameTime), 60.0f, {255, 125, 0, 255});
   }
   renderParticles(getConfettiCluster());
}
