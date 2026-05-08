#pragma once
#include <raylib.h>

struct Particle {
   Texture2D *texture;
   Vector2 position, velocity, acceleration;
   Vector2 size;
   float sizeVelocity = 1.0f;
   float rotation = 0.0f;
   float rotationVelocity = 0.0f;
   float lifetime = 0.0f;
   float age = 0.0f;
};

void updateParticles();
void renderParticles();
void spawnParticles(const Particle &particle, int count);
void spawnParticles(const Particle &minimum, const Particle &maximum, int count);

// presets

void spawnCoinParticles(const Vector2 &position, Texture *texture);
