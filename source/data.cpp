#include "data.hpp"
#include "asset.hpp"
#include "file.hpp"
#include <fstream>
#include <sstream>
#include <unordered_map>

static std::vector<std::string> playerIconContainer;
static std::vector<Vector3> playerColorContainer;
static std::vector<std::string> chapterContainer;
static std::vector<Level> levelContainer;
static std::vector<Tile> tileContainer;

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
   stream >> target;

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
         if (!tileNameMap.count(value)) {
            printf("WARNING: Malformed line: '%s'. No such tile '%s'\n", line.c_str(), value.c_str());
         }
         else {
            level.coinTile = tileNameMap[value];
         }
      }
      else if (field == "width") {
         getFieldAsSimpleValue(stream, value, line, level.sizeX);
      }
      else if (field == "height") {
         getFieldAsSimpleValue(stream, value, line, level.sizeY);
      }
      else if (field == "floor" || field == "map") {
         std::vector<std::vector<size_t>> &container = (field == "map" ? level.map : level.floor);
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

      if (field == "tsize") {
         getFieldAsSimpleValue(stream, value, line, tile.tsize);
      }
      else if (field == "width") {
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
         tile.texture = &getTexture(value);
         tileNameMap[value] = tileContainer.size();
      }
      else {
         printf("WARNING: Malformed line: '%s'. Unexpected field '%s'.\n", line.c_str(), field.c_str());
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

// player data module

static CustomizationData customizationData;

CustomizationData getCustomizationData() {
   return customizationData;
}

CustomizationData loadCustomizationData() {
   CustomizationData data;
   std::fstream file ("data/cd.data", std::ios::in);
   if (!file.is_open()) {
      return data; // default data
   }
   
   file.read(reinterpret_cast<char*>(&data), sizeof(data));
   return data;
}

void saveCustomizationData(CustomizationData data) {
   std::fstream file ("data/cd.data", std::ios::out);
   if (!file.is_open()) {
      return;
   }

   file.write(reinterpret_cast<const char*>(&data), sizeof(data));
   customizationData = data;
}

void loadPlayerData() {
   customizationData = loadCustomizationData();
}

void savePlayerData() {
   saveCustomizationData(customizationData);
}
