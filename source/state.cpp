#include "state.hpp"
#include <cmath>
#include <raylib.h>

constexpr float maxDeltaTime = 0.25f;
constexpr float fixedUpdateDeltaTime = 1.0f / 60.0f;
constexpr float fadeTime = 0.4f;

void State::updateStateLogic() {
   int width = GetScreenWidth();
   int height = GetScreenHeight();

   if (width != lastWidth || height != lastHeight) {
      lastWidth = width;
      lastHeight = height;
      updateResponsiveness();
   }

   if (fadingIn) {
      updateFadingIn();
   }
   else if (fadingOut) {
      updateFadingOut();
   }
   else {
      accumulator += fminf(maxDeltaTime, GetFrameTime());
      while (accumulator >= fixedUpdateDeltaTime) {
         fixedUpdate();
         accumulator -= fixedUpdateDeltaTime;
      }
      update();
   }
}

void State::updateFadingIn() {
   fadeTimer += GetFrameTime();
   alpha = 1.f - fadeTimer / fadeTime;

   if (fadeTimer >= fadeTime) {
      fadeTimer = 0.f;
      alpha = 0.f;
      fadingIn = false;
   }
}

void State::updateFadingOut() {
   fadeTimer += GetFrameTime();
   alpha = fadeTimer / fadeTime;

   if (fadeTimer >= fadeTime) {
      alpha = 1.f;
      fadingOut = false;
      quittingState = true;
   }
}
