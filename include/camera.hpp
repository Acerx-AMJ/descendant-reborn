#pragma once
#include "player.hpp"
#include <raylib.h>

struct CameraDR3 {
   void init(Player *lock, Rectangle bounds, Vector2 center, float zoom, float lerpSpeed, float zoomSpeed, float zoomMin, float zoomMax);
   void shake(float strength, float length, float frequency = 20.0f);
   void update();

   Camera2D camera;
   Rectangle bounds;
   Vector2 target;
   Vector2 velocity;
   Player *lock = nullptr;

   float targetZoom = 1.0f;
   float shakeStrength = 0.0f;
   float shakeTimer = 0.0f;
   float shakeFrequency = 0.0f;
   float lastUpdate = 0.0f;
   float lerpSpeed = 0.0f;
   float zoomSpeed = 0.0f;
   float zoomMin = 0.0f;
   float zoomMax = 0.0f;
};
