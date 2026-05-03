#include "camera.hpp"
#include "input.hpp"
#include "math.hpp"
#include "render.hpp"
#include <raymath.h>

constexpr float shakeFrequency = 1.0f / 20.0f;
constexpr float lerpSpeed = 0.15f;
constexpr float zoomSpeed = 0.15f;

void CameraDR3::init(Player *lock, Rectangle bounds, Vector2 center, float zoom, float zoomMin, float zoomMax) {
   camera.target = center;
   camera.offset = getScreenCenter();
   camera.rotation = 0.0f;
   camera.zoom = zoom;
   this->lock = lock;
   this->bounds = bounds;
   this->targetZoom = zoom;
   this->zoomMin = zoomMin;
   this->zoomMax = zoomMax;
}

void CameraDR3::shake(float strenght, float length) {
   shakeStrength = strenght;
   shakeTimer = length;
}

void CameraDR3::update() {
   Vector2 position = target;

   if (lock) {
      target = lock->position;
      position = Vector2Lerp(camera.target, target, lerpSpeed);
   }

   // Bounds
   Vector2 halfSize = {camera.offset.x / camera.zoom, camera.offset.y / camera.zoom};

   if ((camera.offset.x * 2) / camera.zoom >= bounds.width) {
      position.x = bounds.x + bounds.width * 0.5f;
   }
   else {
      position.x = Clamp(position.x, bounds.x + halfSize.x, bounds.x + bounds.width - halfSize.x);
   }

   if ((camera.offset.y * 2) / camera.zoom >= bounds.height) {
      position.y = bounds.y + bounds.height * 0.5f;
   }
   else {
      position.y = Clamp(position.y, bounds.y + halfSize.y, bounds.y + bounds.height - halfSize.y);
   }

   // Shake
   if (shakeTimer > 0.0f) {
      shakeTimer -= GetFrameTime();
      lastUpdate += GetFrameTime();

      if (lastUpdate >= shakeFrequency) {
         lastUpdate -= shakeFrequency;
         position.x += sin(randomFloat(0.0f, 6.28f)) * shakeStrength * shakeTimer;
         position.y += cos(randomFloat(0.0f, 6.28f)) * shakeStrength * shakeTimer;
      }
   }

   velocity = Vector2Subtract(position, camera.target);
   camera.target = position;

   // Zoom
   targetZoom = Clamp(targetZoom, zoomMin, zoomMax);
   float newZoom = camera.zoom;

   if (camera.zoom != targetZoom) {
      newZoom = Lerp(camera.zoom, targetZoom, zoomSpeed);
   }

   if (newZoom != camera.zoom) {
      camera.zoom = Clamp(newZoom, zoomMin, zoomMax);
   }
}

void CameraDR3::updateControls() {
   float scrollDelta = GetMouseWheelMove();
   if (scrollDelta >= 0.5f) {
      targetZoom *= 1.0f - GetFrameTime() * 6.0f;
   }
   else if (scrollDelta <= -0.5f) {
      targetZoom *= 1.0f + GetFrameTime() * 6.0f;
   }

   if (isKeyRepeated(KEY_EQUAL)) {
      targetZoom *= 0.7f;
   }
   else if (isKeyRepeated(KEY_MINUS)) {
      targetZoom *= 1.3f;
   }
}
