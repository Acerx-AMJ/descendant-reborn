#include "map.hpp"
#include "camera.hpp"
#include "math.hpp"
#include "player.hpp"
#include "render.hpp"
#include <raymath.h>

constexpr float tileSize = 50.0f;

void Map::init(const Level &level, CameraDR3 &camera, Player &player) {
   Rectangle bounds = {0, 0, level.sizeX * tileSize, level.sizeY * tileSize};
   std::vector<Vector2> spawnPositions;

   tiles = std::vector<std::vector<Tile>>(level.sizeY, std::vector<Tile>(level.sizeX));
   floor = std::vector<std::vector<Tile>>(level.sizeY, std::vector<Tile>(level.sizeX));

   for (size_t y = 0; y < level.sizeY; ++y) {
      for (size_t x = 0; x < level.sizeX; ++x) {
         size_t tileID = level.map[y][x];

         if (tileID == 0) {
            continue;
         }

         if (tileID == 1) {
            spawnPositions.push_back({(float)x, (float)y});
            continue;
         }

         size_t realID = tileID - 2;

         if (realID >= getTileCount()) {
            printf("WARNING: skipping tile with ID %lu since that is an invalid ID. Tile count is %lu.\n", realID, getTileCount());
            continue;
         }

         // tile is taken by a bigger tile already
         if (tiles[y][x].tileType != Tile::TileType::none) {
            continue;
         }

         Tile &tile = getTile(realID);

         // don't place the tile at all if it is out of bounds
         if (x + tile.width > level.sizeX || y + tile.height > level.sizeY) {
            continue;
         }

         tiles[y][x] = tile;
         tiles[y][x].tileType = Tile::TileType::root;
         coinCount += (tile.type == Tile::Type::coin);

         for (size_t yy = 0; yy < tile.height; ++yy) {
            for (size_t xx = 0; xx < tile.width; ++xx) {
               // root tile
               if (yy == 0 && xx == 0) {
                  continue;
               }

               tiles[yy][xx].tileType = Tile::TileType::ghost;
               tiles[yy][xx].rootPosition = {(float)x, (float)y};
            }
         }
      }
   }

   for (size_t y = 0; y < level.sizeY; ++y) {
      for (size_t x = 0; x < level.sizeX; ++x) {
         size_t tileID = level.floor[y][x];

         if (tileID < 2) {
            continue;
         }

         size_t realID = tileID - 2;

         if (realID >= getTileCount()) {
            printf("WARNING: skipping tile with ID %lu since that is an invalid ID. Tile count is %lu.\n", realID, getTileCount());
            continue;
         }

         // tile is taken by a bigger tile already
         if (floor[y][x].tileType != Tile::TileType::none) {
            continue;
         }

         Tile &tile = getTile(realID);

         // don't place the tile at all if it is out of bounds
         if (x + tile.width > level.sizeX || y + tile.height > level.sizeY) {
            continue;
         }

         floor[y][x] = tile;
         floor[y][x].tileType = Tile::TileType::root;

         for (size_t yy = 0; yy < tile.height; ++yy) {
            for (size_t xx = 0; xx < tile.width; ++xx) {
               // root tile
               if (yy == 0 && xx == 0) {
                  continue;
               }

               floor[yy + y][xx + x].tileType = Tile::TileType::ghost;
               floor[yy + y][xx + x].rootPosition = {(float)x, (float)y};
            }
         }
      }
   }

   if (spawnPositions.empty()) {
      printf("ERROR: Cannot play the game without a player.\n");
      exit(EXIT_FAILURE);
   }

   Vector2 spawnPosition = randomVectorAccess(spawnPositions);
   player.position = Vector2Add(spawnPosition, getOrigin(playerSize));
   player.init(bounds, getCustomizationData());
   camera.init(&player, bounds, spawnPosition, 0.75f, 0.25f, 4.0f);
   camera.update();

   name = level.name;
   chapter = level.chapter;
   coinTile = level.coinTile;
   sizeX = level.sizeX;
   sizeY = level.sizeY;
}

void Map::render(Player &player) {
   for (size_t y = 0; y < sizeY; ++y) {
      for (size_t x = 0; x < sizeX; ++x) {
         Tile &tile = floor[y][x];

         if (tile.tileType == Tile::TileType::root) {
            drawTexture(*tile.texture, Vector2Scale({(float)x, (float)y}, tileSize), Vector2Scale({(float)tile.width, (float)tile.height}, tileSize), {95, 95, 135, 255});
         }
      }
   }

   for (size_t y = 0; y < sizeY; ++y) {
      for (size_t x = 0; x < sizeX; ++x) {
         Tile &tile = tiles[y][x];

         if (tile.tileType == Tile::TileType::root) {
            drawTexture(*tile.texture, Vector2Scale({(float)x, (float)y}, tileSize), Vector2Scale({(float)tile.width, (float)tile.height}, tileSize), WHITE);
         }
      }
   }

   player.render();
}
