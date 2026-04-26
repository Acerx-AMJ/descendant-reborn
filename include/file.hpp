#pragma once
#include <raylib.h>
#include <string>
#include <vector>

std::vector<std::string> getLinesFromFileIgnoringCommentsAndEmptyLines(const std::string &path);
std::string getRandomLineFromFile(const std::string &path);
std::string wrap(const std::string &string, Font font, float maxWidth, float fontSize, float spacing);
