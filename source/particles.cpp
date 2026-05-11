#include "particles.hpp"
#include "asset.hpp"
#include "math.hpp"
#include "render.hpp"
#include "state.hpp"
#include <raymath.h>
#include <algorithm>
#include <vector>

static constexpr size_t particlePresetCount = 2;
static std::vector<std::vector<Particle>> particles (particlePresetCount);

void updateParticles() {
   for (std::vector<Particle> &cluster: particles) {
      for (Particle &particle: cluster) {
         particle.age += fixedUpdateDeltaTime;
         particle.position += particle.velocity * fixedUpdateDeltaTime;
         particle.velocity += particle.acceleration * fixedUpdateDeltaTime;
         particle.size += V2(particle.sizeVelocity * fixedUpdateDeltaTime);
         particle.rotation += particle.rotationVelocity * fixedUpdateDeltaTime;
      }

      cluster.erase(std::remove_if(cluster.begin(), cluster.end(), [](const Particle &p) -> bool {
         return p.age >= p.lifetime;
      }), cluster.end());
   }
}

void renderParticles(std::vector<Particle> &cluster) {
   for (Particle &particle: cluster) {
      Vector2 size = particle.size * getCubicRatio();
      DrawTexturePro(*particle.texture, getSource(*particle.texture), getRectangle(particle.position, size), getOrigin(size), particle.rotation, Fade(WHITE, 1.0f - particle.age / particle.lifetime));
   }
}

void spawnParticles(const Particle &minimum, const Particle &maximum, size_t count, size_t ID) {
   std::vector<Particle> cluster;
   cluster.reserve(count);

   for (size_t i = 0; i < count; ++i) {
      Particle particle = {
         minimum.texture,
         {randomFloat(minimum.position.x, maximum.position.x), randomFloat(minimum.position.y, maximum.position.y)},
         {randomFloat(minimum.velocity.x, maximum.velocity.x), randomFloat(minimum.velocity.y, maximum.velocity.y)},
         {randomFloat(minimum.acceleration.x, maximum.acceleration.x), randomFloat(minimum.acceleration.y, maximum.acceleration.y)},
         {randomFloat(minimum.size.x, maximum.size.x), randomFloat(minimum.size.y, maximum.size.y)},
         randomFloat(minimum.sizeVelocity, maximum.sizeVelocity),
         randomFloat(minimum.rotation, maximum.rotation),
         randomFloat(minimum.rotationVelocity, maximum.rotationVelocity),
         randomFloat(minimum.lifetime, maximum.lifetime),
         0.0f,
      };
      cluster.push_back(particle);
   }
   particles[ID] = cluster;
}

// presets

void spawnCoinParticles(const Vector2 &position, Texture *texture) {
   spawnParticles({
      texture,
      position - V2(1.0f) * getCubicRatio(),
      {-200.0f, -200.0f},
      {0.0f, 500.0f},
      {12.5f, 12.5f},
      0.95f,
      0.0f,
      -180.0f,
      0.6f
   }, {
      texture,
      position + V2(1.0f) * getCubicRatio(),
      {200.0f, 200.0f},
      {0.0f, 1000.0f},
      {17.5f, 17.5f},
      1.05f,
      360.0f,
      180.0f,
      0.8f
   }, 8, 0);
}

void spawnStarParticles(const Vector2 &position) {
   spawnParticles({
      &getTexture("star"),
      position - V2(25.0f) * getCubicRatio(),
      {-500.0f, -500.0f},
      {0.0f, 500.0f},
      {25.0f, 25.0f},
      0.92f,
      0.0f,
      -180.0f,
      1.0f,
   }, {
      &getTexture("star"),
      position + V2(25.0f) * getCubicRatio(),
      {500.0f, 500.0f},
      {0.0f, 1000.0f},
      {35.0f, 35.0f},
      0.98f,
      360.0f,
      180.0f,
      2.0f,
   }, 25, 1);
}

std::vector<Particle> &getCoinParticleCluster() {
   return particles[0];
}

std::vector<Particle> &getStarParticleCluster() {
   return particles[1];
}
