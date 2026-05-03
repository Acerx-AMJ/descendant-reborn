#pragma once
#include "data.hpp"
#include <array>
#include <raylib.h>

static inline constexpr std::size_t shadowCount = 3;
static inline constexpr Vector2 playerSize = {50.0f, 50.0f};

struct Player {
   void init(Rectangle bounds, CustomizationData data);
   void update(); // to be called in 'fixedUpdate'
   void render();

   // Members

   struct Shadow {
      Vector2 position;
      Vector2 size;
      float alpha;
   };

   std::array<Shadow, shadowCount> shadows;
   Rectangle bounds;
   Vector2 direction;
   Vector2 position;

   std::size_t iconID = 0;
   std::size_t primaryColorID = 0;
   std::size_t secondaryColorID = 0;

   float playerSpeed = 5.0f;
   bool playerInitialized = false;
   bool shadowsEnabled = true;
   bool blockMovement = false;

   int primaryShaderLocation = 0;
   int secondaryShaderLocation = 0;
};
