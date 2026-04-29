#pragma once
#include <raylib.h>
#include <string>
#include <vector>

// game data module

void loadData();
void loadPlayerIcons();
void loadPlayerColors();

size_t getPlayerIconCount();
std::string getPlayerIcon(size_t ID);
std::vector<std::string> &getPlayerIconContainer();

size_t getPlayerColorCount();
Vector3 getPlayerColor(size_t ID);
std::vector<Vector3> &getPlayerColorContainer();

// player data module

struct CustomizationData {
   size_t iconID = 0;
   size_t primaryColorID = 1;
   size_t secondaryColorID = 2;
   size_t magicNumber = 0xba5eba11;
};

CustomizationData getCustomizationData();
CustomizationData loadCustomizationData();
void saveCustomizationData(CustomizationData data);

void loadPlayerData();
void savePlayerData();

