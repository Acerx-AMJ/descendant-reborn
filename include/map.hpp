#pragma once
#include "data.hpp"

struct Level;
struct CameraDR3;
struct Player;

struct Map {
   void init(const Level &level, CameraDR3 &camera, Player &player);
   void render(Player &player);

   std::string name;
   std::string chapter;
   size_t coinCount = 0;
   size_t coinTile = 0;
   size_t sizeX = 0;
   size_t sizeY = 0;
   std::vector<std::vector<Tile>> tiles;
   std::vector<std::vector<Tile>> floor;
};
