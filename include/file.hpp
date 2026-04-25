#pragma once
#include <raylib.h>
#include <string>

std::string getRandomLineFromFile(const std::string &path);
std::string wrap(const std::string &string, Font font, float maxWidth, float fontSize, float spacing);
