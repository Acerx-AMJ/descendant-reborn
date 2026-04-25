#pragma once
#include <cstdlib>
#include <vector>

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
