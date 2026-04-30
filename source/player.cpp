#include "player.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "render.hpp"
#include <raylib.h>
#include <raymath.h>

void Player::init(Rectangle bounds) {
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

   Vector2 target = Vector2Scale(direction, playerSpeed);
   position = Vector2Add(position, target);
   position.x = Clamp(position.x, bounds.x, bounds.x + bounds.width);
   position.y = Clamp(position.y, bounds.y, bounds.y + bounds.height);
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
