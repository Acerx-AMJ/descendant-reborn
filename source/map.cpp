#include "map.hpp"
#include "asset.hpp"
#include "camera.hpp"
#include "math.hpp"
#include "particles.hpp"
#include "player.hpp"
#include "render.hpp"
#include "sound.hpp"
#include <raymath.h>

void Map::init(const Level &level, CameraAA3 &camera, Player &player) {
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
            spawnPositions.push_back(V2(x, y) * tileSize);
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
         initAnimationIfExists(tiles[y][x].texture, tiles[y][x].texture.name);

         for (size_t yy = 0; yy < tile.height; ++yy) {
            for (size_t xx = 0; xx < tile.width; ++xx) {
               // root tile
               if (yy == 0 && xx == 0) {
                  continue;
               }

               tiles[yy + y][xx + x].tileType = Tile::TileType::ghost;
               tiles[yy + y][xx + x].rootPosition = {(float)x, (float)y};
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
         initAnimationIfExists(floor[y][x].texture, floor[y][x].texture.name);

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
   levelID = level.ID;
   coinTile = level.coinTile;
   sizeX = level.sizeX;
   sizeY = level.sizeY;
   time = level.time;
   perfectTime = level.perfectTime;
}

void Map::render(Player &player, const Rectangle &bounds, bool paused) {
   float coinOffset = sin(GetTime() * 0.75f) * (5.0f / tileSize);
   
   drawnRootTiles.clear();
   for (size_t y = bounds.y; y <= bounds.height; ++y) {
      for (size_t x = bounds.x; x <= bounds.width; ++x) {
         Tile &tile = floor[y][x];

         if (tile.tileType == Tile::TileType::root) {
            drawTextureAnimated(tile.texture, V2(x, y) * tileSize, V2(tile.width, tile.height) * tileSize, floorColor, paused);
         }
         else if (tile.tileType == Tile::TileType::ghost && (tile.rootPosition.x < bounds.x || tile.rootPosition.y < bounds.y)) {
            Vector2 pos = tile.rootPosition;
            if (drawnRootTiles.count(pos)) {
               continue;
            }

            drawnRootTiles.insert(pos);
            Tile &tile = floor[pos.y][pos.x];
            drawTextureAnimated(tile.texture, pos * tileSize, V2(tile.width, tile.height) * tileSize, floorColor, paused);
         }
      }
   }

   drawnRootTiles.clear();
   for (size_t y = bounds.y; y <= bounds.height; ++y) {
      for (size_t x = bounds.x; x <= bounds.width; ++x) {
         Tile &tile = tiles[y][x];

         if (tile.tileType == Tile::TileType::root) {
            Vector2 position = V2(x, y + (tile.type == Tile::Type::coin) * coinOffset) * tileSize;
            drawTextureAnimated(tile.texture, position, V2(tile.width, tile.height) * tileSize, WHITE, paused);
         }
         else if (tile.tileType == Tile::TileType::ghost && (tile.rootPosition.x < bounds.x || tile.rootPosition.y < bounds.y)) {
            Vector2 pos = tile.rootPosition;
            if (drawnRootTiles.count(pos)) {
               continue;
            }

            drawnRootTiles.insert(pos);
            Tile &tile = tiles[pos.y][pos.x];

            pos.y += (tile.type == Tile::Type::coin) * coinOffset;
            drawTextureAnimated(tile.texture, pos * tileSize, V2(tile.width, tile.height) * tileSize, WHITE, paused);
         }
      }
   }

   renderParticles(getCoinParticleCluster());
   player.render();
}

void Map::removeTile(size_t x, size_t y) {
   // safe guard against ghost tiles
   if (tiles[y][x].tileType == Tile::TileType::ghost) {
      Vector2 rootPosition = tiles[y][x].rootPosition;
      x = rootPosition.x;
      y = rootPosition.y;
   }

   Vector2 rootPosition = V2(x, y);
   tiles[y][x] = {};

   for (size_t yy = y; yy < sizeY; ++yy) {
      for (size_t xx = x; xx < sizeX; ++xx) {
         // skip root tile
         if (yy == y && xx == x) {
            continue;
         }

         Tile &tile = tiles[yy][xx];
         if (tile.tileType != Tile::TileType::ghost || tile.rootPosition != rootPosition) {
            break;
         }
         tile = {};
      }
   }
}

void Map::collectCoin(size_t x, size_t y) {
   if (tiles[y][x].type == Tile::Type::coin) {
      spawnCoinParticles(V2(x + 0.5f, y + 0.5f) * tileSize, &getTexture(tiles[y][x].texture.name));
      playSound("coin");
      removeTile(x, y);
   }
}
