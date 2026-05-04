#pragma once
#include <cstdlib>
#include <raylib.h>
#include <vector>

template<typename T, typename U>
constexpr inline Vector2 V2(T x, U y) {
   return {(float)x, (float)y};
}

template<typename T>
constexpr inline Vector2 V2(T value) {
   return {(float)value, (float)value};
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
