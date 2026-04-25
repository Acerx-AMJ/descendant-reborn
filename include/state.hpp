#pragma once

struct State {
   State() = default;
   virtual ~State() = default;

   virtual void update() = 0;
   virtual void render() = 0;
   virtual void fixedUpdate() = 0;
   virtual void updateResponsiveness() = 0;
   virtual State *change() = 0;

   void updateStateLogic();
   void updateFadingIn();
   void updateFadingOut();

   int lastWidth = 0;
   int lastHeight = 0;

   bool quittingState = false;
   bool fadingIn = false;
   bool fadingOut = false;

   float fadeTimer = 0.0f;
   float alpha = 0.0f;
   float accumulator = 0.0f;
};
