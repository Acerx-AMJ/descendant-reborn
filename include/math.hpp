#pragma once
#include <cstdlib>
#include <raylib.h>
#include <vector>

template<typename T, typename Y>
constexpr inline Vector2 V2(T x, Y y) {
   return {(float)x, (float)y};
}

template<typename T>
constexpr inline Vector2 V2(T value) {
   return {(float)value, (float)value};
}

template<typename T, typename Y, typename U, typename I>
constexpr inline Rectangle R4(T x, Y y, U width, I height) {
   return {(float)x, (float)y, (float)width, (float)height};
}

template<typename T, typename Y, typename U, typename I>
constexpr inline Color C4(T r, Y g, U b, I a) {
   return {(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a};
}

inline int randomInt(int min, int max) {
   return min + (rand() % (max - min + 1));
}

inline float randomFloat(float min, float max) {
   return min + (float)rand() / float(RAND_MAX / (max - min));
}

inline Vector2 randomV2(Vector2 min, Vector2 max) {
   return {randomFloat(min.x, max.x), randomFloat(min.y, max.y)};
}

inline Vector2 randomV2(float min, float max) {
   return V2(randomFloat(min, max));
}

inline bool chance(int percent) {
   return randomInt(1, 100) <= percent;
}

template<class T>
inline T &randomVectorAccess(std::vector<T> &vector) {
   return vector[rand() % vector.size()];
}

template<class T>
inline const T &randomVectorAccess(const std::vector<T> &vector) {
   return vector.at(rand() % vector.size());
}

template<class T>
inline T &randomVectorAccess(std::vector<T> &&vector) {
   return vector[rand() % vector.size()];
}
