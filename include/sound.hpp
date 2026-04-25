#pragma once
#include <raylib.h>
#include <string>

void playSound(const std::string &name, float volume = 1.0f);
void loadSound(const std::string &name, const std::string &path);
void loadSounds();
