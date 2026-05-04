#include "player.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "map.hpp"
#include "math.hpp"
#include "render.hpp"
#include <raylib.h>
#include <raymath.h>

void Player::init(Rectangle bounds, CustomizationData data) {
   iconID = data.iconID;
   primaryColorID = data.primaryColorID;
   secondaryColorID = data.secondaryColorID;
   shadowsEnabled = data.shadowsEnabled;

   // we do this transformation to make collision against bounds two simple clamps
   Vector2 origin = getOrigin(playerSize);
   this->bounds = getRectangle(Vector2Add({bounds.x, bounds.y}, origin), Vector2Subtract({bounds.width, bounds.height}, playerSize));

   Shader shader = getShader("twocolor");
   primaryShaderLocation = GetShaderLocation(shader, "primary");
   secondaryShaderLocation = GetShaderLocation(shader, "secondary");
   playerInitialized = true;

   float shadowAlphaUnit = 1.0f / (shadowCount + 1);
   for (std::size_t i = 0; i < shadowCount; ++i) {
      Shadow shadow;
      shadow.position = position;
      shadow.size = Vector2SubtractValue(playerSize, (i + 1) * 5.0f);
      shadow.alpha = 1.0f - (i + 1) * shadowAlphaUnit - shadowAlphaUnit * 0.5f;
      shadows[i] = shadow;
   }
}

void Player::update() {
   if (Vector2DistanceSqr(shadows.back().position, position) > 1.0f) {
      for (std::size_t i = 0; i < shadowCount; ++i) {
         shadows[i].position = Vector2Lerp(shadows[i].position, position, 0.0458f * (shadowCount - i));
      }
   }
   
   float directionX = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
   float directionY = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
   direction = Vector2Normalize({directionX, directionY});

   if ((direction.x == 0.0f && direction.y == 0.0f) || !playerInitialized || blockMovement) {
      return;
   }

   position = Vector2Add(position, Vector2Scale(direction, playerSpeed));
   position.x = Clamp(position.x, bounds.x, bounds.x + bounds.width);
   position.y = Clamp(position.y, bounds.y, bounds.y + bounds.height);
}

void Player::update(Map &map) {
   if (Vector2DistanceSqr(shadows.back().position, position) > 1.0f) {
      for (std::size_t i = 0; i < shadowCount; ++i) {
         shadows[i].position = Vector2Lerp(shadows[i].position, position, 0.0458f * (shadowCount - i));
      }
   }
   
   float directionX = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
   float directionY = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
   direction = Vector2Normalize({directionX, directionY});

   if (!playerInitialized || blockMovement) {
      return;
   }

   if (direction.x != 0.0f) {
      position.x += direction.x * playerSpeed;
      position.x = Clamp(position.x, bounds.x, bounds.x + bounds.width);
      Rectangle playerBounds = getRectangle(position - getOrigin(playerSize), playerSize);
      size_t startX = playerBounds.x / tileSize;
      size_t startY = playerBounds.y / tileSize;
      size_t endX = fmin(startX + 2, map.sizeX - 1);
      size_t endY = fmin(startY + 2, map.sizeY - 1);

      for (size_t y = startY; y <= endY; ++y)  {
         for (size_t x = startX; x <= endX; ++x) {
            Tile tile = map.tiles[y][x];
            if (tile.tileType == Tile::TileType::none) {
               continue;
            }

            Vector2 pos = V2(x, y);
            Vector2 size = V2(1.0f, 1.0f);

            if (tile.tileType == Tile::TileType::ghost) {
               pos = tile.rootPosition;
               tile = map.tiles[pos.y][pos.x];
               size = V2(tile.width, tile.height);
            }

            Rectangle tileBounds = getRectangle(pos * tileSize, size * tileSize);
            if (!CheckCollisionRecs(playerBounds, tileBounds)) {
               continue;
            }

            if (tile.type == Tile::Type::coin) {
               map.removeTile(x, y);
               map.collectedCoins += 1;
            }

            if (tile.type == Tile::Type::finish) {

            }

            if (tile.type == Tile::Type::solid) {
               playerBounds.x = (tileBounds.x > playerBounds.x ? tileBounds.x - playerBounds.width : tileBounds.x + tileBounds.width);
            }
         }
      }
      position.x = playerBounds.x + playerSize.x / 2.0f;
   }

   if (direction.y != 0.0f) {
      position.y += direction.y * playerSpeed;
      position.y = Clamp(position.y, bounds.y, bounds.y + bounds.height);
      Rectangle playerBounds = getRectangle(position - getOrigin(playerSize), playerSize);
      size_t startX = playerBounds.x / tileSize;
      size_t startY = playerBounds.y / tileSize;
      size_t endX = fmin(startX + 2, map.sizeX - 1);
      size_t endY = fmin(startY + 2, map.sizeY - 1);

      for (size_t y = startY; y <= endY; ++y)  {
         for (size_t x = startX; x <= endX; ++x) {
            Tile tile = map.tiles[y][x];
            if (tile.tileType == Tile::TileType::none) {
               continue;
            }

            Vector2 pos = V2(x, y);
            Vector2 size = V2(1.0f, 1.0f);

            if (tile.tileType == Tile::TileType::ghost) {
               pos = tile.rootPosition;
               tile = map.tiles[pos.y][pos.x];
               size = V2(tile.width, tile.height);
            }

            Rectangle tileBounds = getRectangle(pos * tileSize, size * tileSize);
            if (!CheckCollisionRecs(playerBounds, tileBounds)) {
               continue;
            }

            if (tile.type == Tile::Type::coin) {
               map.removeTile(x, y);
               map.collectedCoins += 1;
            }

            if (tile.type == Tile::Type::finish) {

            }

            if (tile.type == Tile::Type::solid) {
               playerBounds.y = (tileBounds.y > playerBounds.y ? tileBounds.y - playerBounds.height : tileBounds.y + tileBounds.height);
            }
         }
      }
      position.y = playerBounds.y + playerSize.y / 2.0f;
   }
}

void Player::render() {
   if (!playerInitialized) {
      return;
   }

   Shader shader = getShader("twocolor");
   Texture texture = getTexture(getPlayerIcon(iconID));
   Vector3 primary = getPlayerColor(primaryColorID);
   Vector3 secondary = getPlayerColor(secondaryColorID);

   SetShaderValue(shader, primaryShaderLocation, &primary, SHADER_UNIFORM_VEC3);
   SetShaderValue(shader, secondaryShaderLocation, &secondary, SHADER_UNIFORM_VEC3);

   BeginShaderMode(shader);
      for (int i = shadowCount - 1; shadowsEnabled && i >= 0; --i) {
         drawTextureCentered(texture, shadows[i].position, shadows[i].size, Fade(WHITE, shadows[i].alpha));
      }

      drawTextureCentered(texture, position, playerSize, WHITE);
   EndShaderMode();
}
