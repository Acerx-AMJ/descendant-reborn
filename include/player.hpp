#pragma once
#include <array>
#include <raylib.h>

static inline constexpr std::size_t shadowCount = 3;
static inline constexpr Vector2 playerSize = {50.0f, 50.0f};

static inline constexpr std::size_t playerIconCount = 3;
static inline constexpr std::array<const char*, playerIconCount> playerIconMap {
   "player1", "player2", "player3"
};

static inline constexpr std::size_t playerColorCount = 6;
static inline constexpr std::array<Vector3, playerColorCount> playerColorMap {{
   {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f},
   {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}
}};

struct Player {
   void init();
   void update(); // to be called in 'fixedUpdate'
   void render();

   // Members

   struct Shadow {
      Vector2 position;
      Vector2 size;
      float alpha;
   };

   std::array<Shadow, shadowCount> shadows;
   Vector2 direction;
   Vector2 position;

   std::size_t iconID = 0;
   std::size_t primaryColorID = 0;
   std::size_t secondaryColorID = 0;

   float playerSpeed = 5.0f;
   bool playerInitialized = false;

   int primaryShaderLocation = 0;
   int secondaryShaderLocation = 0;
};
