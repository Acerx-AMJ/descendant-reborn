#pragma once
#include "data.hpp"
#include <unordered_set>

struct Level;
struct CameraAA3;
struct Player;

struct Vector2Hash {
   inline size_t operator () (const Vector2 &vector) const {
      return std::hash<size_t>()(vector.x) ^ (std::hash<size_t>()(vector.y) << 1);
   }
};

constexpr float tileSize = 50.0f;
constexpr Color floorColor = {95, 95, 135, 255};

struct Map {
   void init(const Level &level, CameraAA3 &camera, Player &player);
   void render(Player &player, const Rectangle &bounds, bool paused);

   // utillity

   // remove a tile including its ghost tiles. position supplied does not have to be the root position
   void removeTile(size_t x, size_t y);
   void collectCoin(size_t x, size_t y);

   std::string name;
   std::string chapter;
   std::string coinTile;
   size_t coinCount = 0;
   size_t collectedCoins = 0;
   size_t sizeX = 0;
   size_t sizeY = 0;
   float time = 0.0f;
   float perfectTime = 0.0f;
   std::vector<std::vector<Tile>> tiles;
   std::vector<std::vector<Tile>> floor;
   std::unordered_set<Vector2, Vector2Hash> drawnRootTiles;
};
