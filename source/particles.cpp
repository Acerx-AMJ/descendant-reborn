#include "particles.hpp"
#include "math.hpp"
#include "render.hpp"
#include "state.hpp"
#include <raymath.h>
#include <algorithm>
#include <vector>

static std::vector<Particle> particles;

void updateParticles() {
   for (Particle &particle: particles) {
      particle.age += fixedUpdateDeltaTime;
      particle.position += particle.velocity * fixedUpdateDeltaTime;
      particle.velocity += particle.acceleration * fixedUpdateDeltaTime;
      particle.size += V2(particle.sizeVelocity * fixedUpdateDeltaTime);
      particle.rotation += particle.rotationVelocity * fixedUpdateDeltaTime;
   }

   particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle &p) -> bool {
      return p.age >= p.lifetime;
   }), particles.end());
}

void renderParticles() {
   for (const Particle &particle: particles) {
      DrawTexturePro(*particle.texture, getSource(*particle.texture), {particle.position.x, particle.position.y, particle.size.x, particle.size.y}, getOrigin(particle.size), particle.rotation, Fade(WHITE, 1.0f - particle.age / particle.lifetime));
   }
}

void spawnParticles(const Particle &example, int count) {
   particles.reserve(particles.size() + count);
   for (int i = 0; i < count; ++i)
      particles.push_back(example);
}

void spawnParticles(const Particle &minimum, const Particle &maximum, int count) {
   particles.reserve(particles.size() + count);
   for (int i = 0; i < count; ++i) {
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
      particles.push_back(particle);
   }
}

// presets

void spawnCoinParticles(const Vector2 &position, Texture *texture) {
   spawnParticles({
      texture,
      position - V2(1.0f),
      {-200.0f, -200.0f},
      {0.0f, 500.0f},
      {12.5f, 12.5f},
      0.95f,
      0.0f,
      -180.0f,
      0.6f
   }, {
      texture,
      position + V2(1.0f),
      {200.0f, 200.0f},
      {0.0f, 1000.0f},
      {17.5f, 17.5f},
      1.05f,
      360.0f,
      180.0f,
      0.8f
   }, 8);
}
