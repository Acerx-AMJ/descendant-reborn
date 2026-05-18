#include "particles.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "math.hpp"
#include "render.hpp"
#include "state.hpp"
#include <raymath.h>
#include <algorithm>
#include <vector>

static constexpr size_t particlePresetCount = 4;
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
      if (particle.texture) {
         DrawTexturePro(*particle.texture, getSource(*particle.texture), getRectangle(particle.position, size), getOrigin(size), particle.rotation, Fade(WHITE, 1.0f - particle.age / particle.lifetime));
      }
      else {
         Vector3 colorVector3 = getPlayerColor(particle.colorIndex) * 255.0f;
         Color colorTranslated = C4(colorVector3.x, colorVector3.y, colorVector3.z, (1.0f - particle.age / particle.lifetime) * 255.0f);
         DrawRectanglePro(getRectangle(particle.position, size), getOrigin(size), particle.rotation, colorTranslated);
      }
   }
}

void spawnParticles(const Particle &minimum, const Particle &maximum, size_t count, size_t ID) {
   std::vector<Particle> &cluster = particles[ID];
   cluster.reserve(cluster.size() + count);

   for (size_t i = 0; i < count; ++i) {
      Particle particle = {
         minimum.texture,
         randomV2(minimum.position, maximum.position),
         randomV2(minimum.velocity, maximum.velocity),
         randomV2(minimum.acceleration, maximum.acceleration),
         randomV2(minimum.size.x, maximum.size.x),
         randomFloat(minimum.sizeVelocity, maximum.sizeVelocity),
         randomFloat(minimum.rotation, maximum.rotation),
         randomFloat(minimum.rotationVelocity, maximum.rotationVelocity),
         randomFloat(minimum.lifetime, maximum.lifetime),
         0.0f,
         (minimum.texture ? 0 : rand() % getPlayerColorCount()),
      };
      cluster.push_back(particle);
   }
}

// presets

void spawnCoinParticles(const Vector2 &position, Texture *texture) {
   spawnParticles({
      texture,
      position - V2(5.0f) * getCubicRatio(),
      V2(-200.0f),
      V2(0.0f, 500.0f),
      V2(15.0f),
      0.95f,
      0.0f,
      -180.0f,
      0.6f
   }, {
      texture,
      position + V2(5.0f) * getCubicRatio(),
      V2(200.0f),
      V2(0.0f, 1000.0f),
      V2(20.0f),
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
      V2(-500.0f),
      V2(0.0f, 500.0f),
      V2(25.0f),
      0.92f,
      0.0f,
      -180.0f,
      1.0f,
   }, {
      &getTexture("star"),
      position + V2(25.0f) * getCubicRatio(),
      V2(500.0f),
      V2(0.0f, 1000.0f),
      V2(35.0f),
      0.98f,
      360.0f,
      180.0f,
      2.0f,
   }, 25, 1);
}

void spawnConfetti() {
   spawnParticles({
      nullptr,
      V2(0.0f, -400.0f * getCubicRatio()),
      V2(-400.0f),
      V2(0.0f, 200.0f),
      V2(12.0f),
      0.92f,
      0.0f,
      -180.0f,
      6.0f,
   }, {
      nullptr,
      V2((float)GetScreenWidth(), 0.0f),
      V2(400.0f),
      V2(0.0f, 400.0f),
      V2(24.0f),
      0.98f,
      360.0f,
      180.0f,
      8.0f,
   }, 250, 2);
}

void spawnPlayerDeathParticles(const Vector2 &position, Texture *texture) {
   spawnParticles({
      texture,
      position - V2(5.0f) * getCubicRatio(),
      V2(-300.0f),
      V2(0.0f, 500.0f),
      V2(15.0f),
      0.92f,
      0.0f,
      -180.0f,
      0.5f,
   }, {
      texture,
      position + V2(5.0f) * getCubicRatio(),
      V2(300.0f),
      V2(0.0f, 1000.0f),
      V2(20.0f),
      0.98f,
      360.0f,
      180.0f,
      0.7f
   }, 16, 3);
}

std::vector<Particle> &getCoinParticleCluster() {
   return particles[0];
}

std::vector<Particle> &getStarParticleCluster() {
   return particles[1];
}

std::vector<Particle> &getConfettiCluster() {
   return particles[2];
}

std::vector<Particle> &getPlayerDeathParticleCluster() {
   return particles[3];
}
