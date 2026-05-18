#pragma once
#include <raylib.h>
#include <vector>

struct Particle {
   Texture2D *texture;
   Vector2 position, velocity, acceleration;
   Vector2 size;
   float sizeVelocity = 1.0f;
   float rotation = 0.0f;
   float rotationVelocity = 0.0f;
   float lifetime = 0.0f;
   float age = 0.0f;
   size_t colorIndex = 0; // from player colors
};

void updateParticles();
void renderParticles(std::vector<Particle> &cluster);

// presets

void spawnCoinParticles(const Vector2 &position, Texture *texture);
void spawnStarParticles(const Vector2 &position);
void spawnConfetti();
void spawnPlayerDeathParticles(const Vector2 &position, Texture *texture);

std::vector<Particle> &getCoinParticleCluster();
std::vector<Particle> &getStarParticleCluster();
std::vector<Particle> &getConfettiCluster();
std::vector<Particle> &getPlayerDeathParticleCluster();
