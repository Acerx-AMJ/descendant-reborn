#pragma once
#include <raylib.h>
#include <string>
#include <vector>

// game data module

struct Level {
   std::string name;
   std::string chapter;
   size_t coinTile = 0;
   size_t sizeX = 0;
   size_t sizeY = 0;
   std::vector<std::vector<size_t>> floor;
   std::vector<std::vector<size_t>> map;
};

void loadData();
void loadPlayerIcons();
void loadPlayerColors();
void loadChapters();
void loadLevels();

size_t getPlayerIconCount();
std::string &getPlayerIcon(size_t ID);
std::vector<std::string> &getPlayerIconContainer();

size_t getPlayerColorCount();
Vector3 &getPlayerColor(size_t ID);
std::vector<Vector3> &getPlayerColorContainer();

size_t getChapterCount();
std::string &getChapter(size_t ID);
std::vector<std::string> &getChapterContainer();

size_t getLevelCount();
Level &getLevel(size_t ID);
std::vector<Level> &getLevelContainer();

// player data module

struct CustomizationData {
   size_t iconID = 0;
   size_t primaryColorID = 1;
   size_t secondaryColorID = 2;
   bool shadowsEnabled = true;
};

CustomizationData getCustomizationData();
CustomizationData loadCustomizationData();
void saveCustomizationData(CustomizationData data);

void loadPlayerData();
void savePlayerData();

