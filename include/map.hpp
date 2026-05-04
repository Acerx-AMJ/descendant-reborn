#pragma once
#include "data.hpp"
#include <unordered_set>

struct Level;
struct CameraDR3;
struct Player;

struct Vector2Hash {
   inline size_t operator () (const Vector2 &vector) const {
      return std::hash<size_t>()(vector.x) ^ (std::hash<size_t>()(vector.y) << 1);
   }
};

constexpr float tileSize = 50.0f;
constexpr Color floorColor = {95, 95, 135, 255};

struct Map {
   void init(const Level &level, CameraDR3 &camera, Player &player);
   void render(Player &player, const Rectangle &bounds);

   std::string name;
   std::string chapter;
   size_t coinCount = 0;
   size_t coinTile = 0;
   size_t sizeX = 0;
   size_t sizeY = 0;
   std::vector<std::vector<Tile>> tiles;
   std::vector<std::vector<Tile>> floor;
   std::unordered_set<Vector2, Vector2Hash> drawnRootTiles;
};
