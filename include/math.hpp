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

inline int randomInt(int min, int max) {
   return min + (rand() % (max - min + 1));
}

inline float randomFloat(float min, float max) {
   return min + (float)rand() / float(RAND_MAX / (max - min));
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
