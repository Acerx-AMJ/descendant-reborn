#include "data.hpp"
#include "file.hpp"
#include "math.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "raymath.h"

static std::vector<std::string> playerIconContainer;
static std::vector<Vector3> playerColorContainer;
static std::vector<std::string> chapterContainer;
static std::vector<Level> levelContainer;
static std::vector<Tile> tileContainer;
static std::unordered_map<std::string, Animation> animationContainer;

static constexpr size_t differentPerformanceCount = 6;
static std::vector<std::vector<std::string>> resultLineContainer (differentPerformanceCount + 1);
static std::vector<std::vector<Color>> resultColorSchemeContainer (differentPerformanceCount + 1);

static std::unordered_map<std::string, size_t> tileNameMap;

// game data module

void loadData() {
   printf("Loading player icons from 'data/icons.txt'...\n");
   loadPlayerIcons();
   printf("Loading player colors from 'data/colors.txt'...\n");
   loadPlayerColors();
   printf("Loading chapters from 'data/chapters.txt'...\n");
   loadChapters();
   printf("Loading tiles from 'data/tiles.txt'...\n");
   loadTiles();
   printf("Loading levels from 'data/levels.txt'...\n");
   loadLevels();
   printf("Loading animations from 'data/animations.txt'...\n");
   loadAnimations();
   printf("Loading result lines from 'data/results.txt'...\n");
   loadResults();
   printf("Loading successful!\n");
}

void loadPlayerIcons() {
   playerIconContainer = getLinesFromFileIgnoringCommentsAndEmptyLines("data/icons.txt");
}

void loadPlayerColors() {
   std::vector<std::string> lines = getLinesFromFileIgnoringCommentsAndEmptyLines("data/colors.txt");
   std::stringstream stream;
   playerColorContainer.reserve(lines.size());

   for (const std::string &line: lines) {
      stream.clear();
      stream.str(line);

      Vector3 color;
      stream >> color.x >> color.y >> color.z;

      if (stream.rdbuf()->in_avail() != 0) {
         printf("WARNING: Malformed line: '%s'. %ld excess characters, expected 3 real numbers.\n", line.c_str(), stream.rdbuf()->in_avail());
         continue;
      }
      playerColorContainer.push_back(color);
   }
}

void loadChapters() {
   chapterContainer = getLinesFromFileIgnoringCommentsAndEmptyLines("data/chapters.txt");
}

template<typename T>
void getFieldAsSimpleValue(std::stringstream &stream, const std::string &value, const std::string &line, T &target) {
   stream.clear();
   stream.str(value);
   stream >> std::boolalpha >> target;

   if (stream.rdbuf()->in_avail() != 0) {
      printf("WARNING: Malformed line '%s'. Expected number.\n", line.c_str());
   }
}

void loadLevels() {
   std::vector<std::string> lines = getLinesFromFileIgnoringCommentsAndEmptyLines("data/levels.txt");
   std::stringstream stream;
   levelContainer.reserve(lines.size() / 8); // rough estimate
   Level stack, defaultLevel;
   bool defaulting = false;

   for (const std::string &line: lines) {
      if (line == "[DEFAULT]") {
         defaulting = true;
         continue;
      }
      
      if (line == "[END]") {
         if (defaulting) {
            defaulting = false;
            stack = defaultLevel;
         }
         else {
            stack.ID = levelContainer.size();
            levelContainer.push_back(stack);
            stack = defaultLevel;
         }
         continue;
      }

      size_t equals = line.find('=');
      if (equals == std::string::npos) {
         printf("WARNING: Malformed line: '%s'. Expected '=' character.\n", line.c_str());
         continue;
      }

      Level &level = defaulting ? defaultLevel : stack;
      std::string field = line.substr(0, equals);
      std::string value = line.substr(equals + 1);
      field.erase(field.find_last_not_of(" \n\r\t\v\f") + 1);
      value.erase(0, value.find_first_not_of(" \n\r\t\v\f"));

      if (field == "name") {
         level.name = value;
      }
      else if (field == "chapter") {
         level.chapter = value;
      }
      else if (field == "coinTile") {
         level.coinTile = value;
      }
      else if (field == "timerTile") {
         level.timerTile = value;
      }
      else if (field == "width") {
         getFieldAsSimpleValue(stream, value, line, level.sizeX);
      }
      else if (field == "height") {
         getFieldAsSimpleValue(stream, value, line, level.sizeY);
      }
      else if (field == "time") {
         getFieldAsSimpleValue(stream, value, line, level.time);
      }
      else if (field == "perfectTime") {
         getFieldAsSimpleValue(stream, value, line, level.perfectTime);
      }
      else if (field == "floor" || field == "map") {
         std::vector<std::vector<size_t>> &container = (field == "map" ? level.map : level.floor);
         container.clear();
         container.reserve(level.sizeY);
         stream.clear();
         stream.str(value);

         for (size_t y = 0; y < level.sizeY; ++y) {
            std::vector<size_t> row;
            row.reserve(level.sizeX);

            for (size_t x = 0; x < level.sizeX; ++x) {
               size_t value = 0;
               stream >> value;
         
               row.push_back(value);
            }
            container.push_back(row);
         }

         if (stream.rdbuf()->in_avail() != 0) {
            printf("WARNING: Malformed line: '%s'. Expected %lu number array. Fields 'width' and 'height' must be defined before fields 'floor' and 'map'.\n", line.c_str(), level.sizeX * level.sizeY);
         }
      }
      else {
         printf("WARNING: Malformed line: '%s'. Unexpected field '%s'.\n", line.c_str(), field.c_str());
      }
   }
}

void loadTiles() {
   std::vector<std::string> lines = getLinesFromFileIgnoringCommentsAndEmptyLines("data/tiles.txt");
   std::stringstream stream;
   tileContainer.reserve(lines.size() / 4); // rough estimate
   Tile stack, defaultTile;
   bool defaulting = false;

   for (const std::string &line: lines) {
      if (line == "[DEFAULT]") {
         defaulting = true;
         continue;
      }
      
      if (line == "[END]") {
         if (defaulting) {
            defaulting = false;
            stack = defaultTile;
         }
         else {
            tileContainer.push_back(stack);
            stack = defaultTile;
         }
         continue;
      }

      size_t equals = line.find('=');
      if (equals == std::string::npos) {
         printf("WARNING: Malformed line: '%s'. Expected '=' character.\n", line.c_str());
         continue;
      }

      Tile &tile = defaulting ? defaultTile : stack;
      std::string field = line.substr(0, equals);
      std::string value = line.substr(equals + 1);
      field.erase(field.find_last_not_of(" \n\r\t\v\f") + 1);
      value.erase(0, value.find_first_not_of(" \n\r\t\v\f"));

      if (field == "width") {
         getFieldAsSimpleValue(stream, value, line, tile.width);
      }
      else if (field == "height") {
         getFieldAsSimpleValue(stream, value, line, tile.height);
      }
      else if (field == "type") {
         if (value == "solid") {
            tile.type = Tile::Type::solid;
         }
         else if (value == "coin") {
            tile.type = Tile::Type::coin;
         }
         else if (value == "finish") {
            tile.type = Tile::Type::finish;
         }
         else {
            printf("WARNING: Malformed line: '%s'. Invalid type '%s'.\n", line.c_str(), value.c_str());
         }
      }
      else if (field == "texture") {
         tile.texture.name = value;
         tileNameMap[value] = tileContainer.size();
      }
      else {
         printf("WARNING: Malformed line: '%s'. Unexpected field '%s'.\n", line.c_str(), field.c_str());
      }
   }
}

void loadAnimations() {
   std::vector<std::string> lines = getLinesFromFileIgnoringCommentsAndEmptyLines("data/animations.txt");
   std::stringstream stream;
   tileContainer.reserve(lines.size() / 8); // rough estimate

   std::string name;
   Animation stack, defaultAnim;
   bool defaulting = false;

   for (const std::string &line: lines) {
      if (line == "[DEFAULT]") {
         defaulting = true;
         continue;
      }
      
      if (line == "[END]") {
         if (defaulting) {
            defaulting = false;
            stack = defaultAnim;
         }
         else {
            animationContainer[name] = stack;
            stack = defaultAnim;
         }
         continue;
      }

      size_t equals = line.find('=');
      if (equals == std::string::npos) {
         printf("WARNING: Malformed line: '%s'. Expected '=' character.\n", line.c_str());
         continue;
      }

      Animation &anim = defaulting ? defaultAnim : stack;
      std::string field = line.substr(0, equals);
      std::string value = line.substr(equals + 1);
      field.erase(field.find_last_not_of(" \n\r\t\v\f") + 1);
      value.erase(0, value.find_first_not_of(" \n\r\t\v\f"));

      if (field == "width") {
         getFieldAsSimpleValue(stream, value, line, anim.width);
      }
      else if (field == "height") {
         getFieldAsSimpleValue(stream, value, line, anim.height);
      }
      else if (field == "gap") {
         getFieldAsSimpleValue(stream, value, line, anim.gap);
      }
      else if (field == "frames") {
         getFieldAsSimpleValue(stream, value, line, anim.frames);
      }
      else if (field == "variations") {
         getFieldAsSimpleValue(stream, value, line, anim.variations);
      }
      else if (field == "speed") {
         getFieldAsSimpleValue(stream, value, line, anim.animationSpeed);
      }
      else if (field == "randomStartOffset") {
         getFieldAsSimpleValue(stream, value, line, anim.randomAnimationStart);
      }
      else if (field == "texture") {
         name = value;
      }
      else {
         printf("WARNING: Malformed line: '%s'. Unexpected field '%s'.\n", line.c_str(), field.c_str());
      }
   }
}

void loadResults() {
   std::vector<std::string> lines = getLinesFromFileIgnoringCommentsAndEmptyLines("data/results.txt");
   std::stringstream stream;

   size_t index = 0;
   std::vector<std::string> results;
   std::vector<Color> scheme;
   bool lineMode = true;

   for (const std::string &line: lines) {
      if (line == "[0-STARS]") {
         index = 0;
      }
      else if (line == "[1-STARS]") {
         index = 1;
      }
      else if (line == "[2-STARS]") {
         index = 2;
      }
      else if (line == "[3-STARS]") {
         index = 3;
      }
      else if (line == "[PERFECT]") {
         index = 4;
      }
      else if (line == "[NEW-RECORD]") {
         index = 5;
      }
      else if (line == "[DEFAULT]") {
         index = 6;
      }
      else if (line == "[END]") {
         resultLineContainer[index] = results;
         results.clear();
         resultColorSchemeContainer[index] = scheme;
         scheme.clear();
         index = 0;
      }
      else if (line == "[LINES]") {
         lineMode = true;
      }
      else if (line == "[COLORS]") {
         lineMode = false;
      }
      else if (lineMode) {
         results.push_back(line);
      }
      else {
         stream.clear();
         stream.str(line);

         Vector3 color;
         stream >> color.x >> color.y >> color.z;
         color *= 255;

         if (stream.rdbuf()->in_avail() != 0) {
            printf("WARNING: Malformed line: '%s'. %ld excess characters, expected 3 real numbers.\n", line.c_str(), stream.rdbuf()->in_avail());
            continue;
         }
         scheme.push_back({(unsigned char)color.x, (unsigned char)color.y, (unsigned char)color.z, 255});
      }
   }
}

size_t getPlayerIconCount() {
   return playerIconContainer.size();
}

std::string &getPlayerIcon(size_t ID) {
   if (ID >= getPlayerIconCount()) {
      printf("ERROR: Icon ID out of bounds. Size is %lu while ID - %lu.\n", getPlayerIconCount(), ID);
      exit(EXIT_FAILURE);
   }
   return playerIconContainer[ID];
}

std::vector<std::string> &getPlayerIconContainer() {
   return playerIconContainer;
}

size_t getPlayerColorCount() {
   return playerColorContainer.size();
}

Vector3 &getPlayerColor(size_t ID) {
   if (ID >= getPlayerColorCount()) {
      printf("ERROR: Color ID out of bounds. Size is %lu while ID - %lu.\n", getPlayerColorCount(), ID);
      exit(EXIT_FAILURE);
   }
   return playerColorContainer[ID];
}

std::vector<Vector3> &getPlayerColorContainer() {
   return playerColorContainer;
}

size_t getChapterCount() {
   return chapterContainer.size();
}

std::string &getChapter(size_t ID) {
   if (ID >= getChapterCount()) {
      printf("ERROR: Chapter ID out of bounds. Size is %lu while ID - %lu.\n", getChapterCount(), ID);
      exit(EXIT_FAILURE);
   }
   return chapterContainer[ID];
}

std::vector<std::string> &getChapterContainer() {
   return chapterContainer;
}

size_t getLevelCount() {
   return levelContainer.size();
}

Level &getLevel(size_t ID) {
   if (ID >= getLevelCount()) {
      printf("ERROR: Level ID out of bounds. Size is %lu while ID - %lu.\n", getLevelCount(), ID);
      exit(EXIT_FAILURE);
   }
   return levelContainer[ID];
}

std::vector<Level> &getLevelContainer() {
   return levelContainer;
}

size_t getTileCount() {
   return tileContainer.size();
}

Tile &getTile(size_t ID) {
   if (ID >= getTileCount()) {
      printf("ERROR: Tile ID out of bounds. Size is %lu while ID - %lu.\n", getTileCount(), ID);
      exit(EXIT_FAILURE);
   }
   return tileContainer[ID];
}

std::vector<Tile> &getTileContainer() {
   return tileContainer;
}

size_t getAnimationCount() {
   return animationContainer.size();
}

void initAnimationIfExists(TextureAA3 &texture, const std::string &name) {
   texture.name = name;
   if (hasAnimation(name)) {
      Animation &anim = getAnimation(name);
      texture.frame = 0;
      texture.timer = (anim.randomAnimationStart ? randomFloat(0.0f, anim.animationSpeed) : 0.0f);
      texture.variation = rand() % anim.variations;
   }
}

bool hasAnimation(const std::string &name) {
   return animationContainer.count(name);
}

Animation &getAnimation(const std::string &name) {
   if (!hasAnimation(name)) {
      printf("ERROR: Animation '%s' does not exist.\n", name.c_str());
      exit(EXIT_FAILURE);
   }
   return animationContainer[name];
}

std::unordered_map<std::string, Animation> &getAnimationContainer() {
   return animationContainer;
}

std::string &getRandomResultLineBasedOnPerformance(size_t performance) {
   if (performance >= differentPerformanceCount) {
      performance = 0;
   }
   return randomVectorAccess(resultLineContainer[performance]);
}

std::vector<Color> &getResultColorSchemeBasedOnPerformance(size_t performance) {
   if (performance >= differentPerformanceCount) {
      performance = 0;
   }
   return resultColorSchemeContainer[performance];
}

std::string &getRandomDefaultResultLine() {
   return randomVectorAccess(resultLineContainer.back());
}

std::vector<Color> &getDefaultResultColorScheme() {
   return resultColorSchemeContainer.back();
}

// player data module

static CustomizationData customizationData;
static std::vector<LevelData> levelData;

CustomizationData getCustomizationData() {
   return customizationData;
}

CustomizationData loadCustomizationData() {
   CustomizationData data;
   std::fstream file ("data/cd.data", std::ios::in | std::ios::binary);
   if (!file.is_open()) {
      return data; // default data
   }
   
   file.read(reinterpret_cast<char*>(&data), sizeof(data));
   return data;
}

void saveCustomizationData(CustomizationData data) {
   std::fstream file ("data/cd.data", std::ios::out | std::ios::binary | std::ios::trunc);
   if (!file.is_open()) {
      return;
   }

   file.write(reinterpret_cast<const char*>(&data), sizeof(data));
   customizationData = data;
}

LevelData getLevelData(size_t ID) {
   if (ID >= levelData.size()) {
      printf("ERROR: Attempted to access data of invalid level %lu. Level count is %lu.\n", ID, levelData.size());
      exit(EXIT_FAILURE);
   }
   return levelData[ID];
}

LevelData loadLevelData(size_t ID) {
   LevelData data;
   std::fstream file ("data/levels/" + std::to_string(ID) + ".data", std::ios::in | std::ios::binary);
   if (!file.is_open()) {
      return data; // default data
   }

   file.read(reinterpret_cast<char*>(&data), sizeof(data));
   return data;
}

void saveLevelData(LevelData data, size_t ID) {
   std::fstream file ("data/levels/" + std::to_string(ID) + ".data", std::ios::out | std::ios::binary | std::ios::trunc);
   if (!file.is_open()) {
      return;
   }

   file.write(reinterpret_cast<const char*>(&data), sizeof(data));
   levelData[ID] = data;
}

void saveLevelDataOnNewScore(LevelData newData, bool gotAllCoins, size_t ID) {
   LevelData data, oldData = levelData[ID];
   data.perfect = oldData.perfect || newData.perfect;
   data.time = fmin(oldData.time, newData.time);
   data.zoom = newData.zoom;
   data.stars = fmax(oldData.stars, newData.stars);

   if (oldData.perfect != data.perfect || (oldData.time != data.time && gotAllCoins) || oldData.stars != data.stars) {
      saveLevelData(data, ID);
   }
}

void saveLevelZoom(size_t ID, float zoom) {
   LevelData oldData = levelData[ID];
   if (oldData.zoom != zoom) {
      oldData.zoom = zoom;
      saveLevelData(oldData, ID);
   }
}

void loadPlayerData() {
   std::filesystem::create_directories("data/levels");
   
   customizationData = loadCustomizationData();
   levelData.resize(getLevelCount());
   
   for (size_t i = 0; i < getLevelCount(); ++i) {
      levelData[i] = loadLevelData(i);
   }
}

void savePlayerData() {
   saveCustomizationData(customizationData);
   for (size_t i = 0; i < getLevelCount(); ++i) {
      saveLevelData(levelData[i], i);
   }
}
