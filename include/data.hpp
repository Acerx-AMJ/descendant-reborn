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

struct Tile {
   enum class TileType: char {root, ghost, none};
   enum class Type: char {solid, coin, finish};

   TileType tileType = TileType::none;
   Type type = Type::solid;
   size_t ID = 0;
   size_t tsize = 0;
   size_t width = 0;
   size_t height = 0;
   Texture2D *texture = nullptr;
   Vector2 rootPosition;
};

void loadData();
void loadPlayerIcons();
void loadPlayerColors();
void loadChapters();
void loadLevels();
void loadTiles();

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

size_t getTileCount();
Tile &getTile(size_t ID);
std::vector<Tile> &getTileContainer();

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

