#pragma once
#include <raylib.h>
#include <string>
#include <vector>

void loadData();
void loadPlayerIcons();
void loadPlayerColors();

size_t getPlayerIconCount();
std::string getPlayerIcon(size_t ID);
std::vector<std::string> &getPlayerIconContainer();

size_t getPlayerColorCount();
Vector3 getPlayerColor(size_t ID);
std::vector<Vector3> &getPlayerColorContainer();
