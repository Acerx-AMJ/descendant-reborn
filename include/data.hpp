#pragma once
#include <limits>
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>

// game data module

static constexpr size_t chapterCount = 7;
static constexpr size_t differentPerformanceCount = 6;

struct ChapterData {
   std::vector<size_t> levels;
   size_t completedLevels = 0;
   int starCount = 0;
   float totalTime = std::numeric_limits<float>::max();
   bool completed = true;
};

struct Level {
   std::string name;
   std::string coinTile;
   std::string timerTile;
   size_t chapter = 0;
   size_t ID = 0;
   size_t sizeX = 0;
   size_t sizeY = 0;
   float time = 0.0f;
   float perfectTime = 0.0f;
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
   enum class Type: char {solid, coin, finish, deadly};

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
void loadLevels();
void loadTiles();
void loadAnimations();
void loadResults();

size_t getPlayerIconCount();
std::string &getPlayerIcon(size_t ID);
std::vector<std::string> &getPlayerIconContainer();

size_t getPlayerColorCount();
Vector3 &getPlayerColor(size_t ID);
std::vector<Vector3> &getPlayerColorContainer();

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

std::string &getRandomResultLineBasedOnPerformance(size_t performance);
std::vector<Color> &getResultColorSchemeBasedOnPerformance(size_t performance);

std::string &getRandomDefaultResultLine();
std::vector<Color> &getDefaultResultColorScheme();

const char *getChapterCodeName(size_t chapter);
ChapterData &getChapterData(size_t chapter);

// player data module

struct CustomizationData {
   size_t iconID = 0;
   size_t primaryColorID = 1;
   size_t secondaryColorID = 2;
   bool shadowsEnabled = true;
};

struct LevelData {
   bool completed = false;
   bool perfect = false;
   float time = std::numeric_limits<float>::max();
   float zoom = 1.0f;
   int stars = 0;
   int deaths = 0;
};

constexpr inline bool operator == (LevelData lhs, LevelData rhs) {
   return lhs.completed == rhs.completed && lhs.perfect == rhs.perfect && lhs.time == rhs.time
       && lhs.zoom == rhs.zoom && lhs.stars == rhs.stars && lhs.deaths == rhs.deaths;
}

CustomizationData getCustomizationData();
CustomizationData loadCustomizationData();
void saveCustomizationData(CustomizationData data);

LevelData getLevelData(size_t ID);
LevelData loadLevelData(size_t ID);
void saveLevelData(LevelData data, size_t ID);
void saveLevelDataOnNewScore(LevelData newData, bool gotAllCoins, size_t ID);
void saveLevelZoom(size_t ID, float zoom);
void incrementLevelDeathCount(size_t ID);

void loadPlayerData();
void savePlayerData();

void recalculateChapterData();
