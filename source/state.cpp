#include "state.hpp"
#include "input.hpp"
#include "particles.hpp"
#include <cmath>
#include <raylib.h>

void State::updateStateLogic() {
   int width = GetScreenWidth();
   int height = GetScreenHeight();
   updateInput();

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
      update();
   }

   accumulator += fminf(maxDeltaTime, GetFrameTime());
   while (accumulator >= fixedUpdateDeltaTime) {
      fixedUpdate();
      updateParticles();
      accumulator -= fixedUpdateDeltaTime;
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
