#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>

// game data module

struct Level {
   std::string name;
   std::string chapter;
   std::string coinTile;
   std::string timerTile;
   size_t sizeX = 0;
   size_t sizeY = 0;
   std::vector<std::vector<size_t>> floor;
   std::vector<std::vector<size_t>> map;
};

struct TextureAA3 {
   std::string name;
   size_t variation = 0;
   size_t frame = 0;
   float timer = 0.0f;
};

struct Animation {
   size_t width = 0;
   size_t height = 0;
   size_t gap = 0;
   size_t frames = 0;
   size_t variations = 1;
   float animationSpeed = 0.0f;
   bool randomAnimationStart = false;
};

struct Tile {
   enum class TileType: char {root, ghost, none};
   enum class Type: char {solid, coin, finish};

   TileType tileType = TileType::none;
   Type type = Type::solid;
   size_t ID = 0;
   size_t width = 0;
   size_t height = 0;
   TextureAA3 texture;
   Vector2 rootPosition;
};

void loadData();
void loadPlayerIcons();
void loadPlayerColors();
void loadChapters();
void loadLevels();
void loadTiles();
void loadAnimations();

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

size_t getAnimationCount();
void initAnimationIfExists(TextureAA3 &texture, const std::string &name);
bool hasAnimation(const std::string &name);
Animation &getAnimation(const std::string &name);
std::unordered_map<std::string, Animation> &getAnimationContainer();

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

