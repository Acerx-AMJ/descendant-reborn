#include "data.hpp"
#include "file.hpp"
#include <fstream>
#include <sstream>

static std::vector<std::string> playerIconContainer;
static std::vector<Vector3> playerColorContainer;
static std::vector<std::string> chapterContainer;
static std::vector<Level> levelContainer;

// game data module

void loadData() {
   loadPlayerIcons();
   loadPlayerColors();
   loadChapters();
   loadLevels();
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
         printf("WARNING: Malformed line in 'data/colors.txt': '%s'. %ld excess characters, expected 3 real numbers.\n", line.c_str(), stream.rdbuf()->in_avail());
         continue;
      }
      playerColorContainer.push_back(color);
   }
}

void loadChapters() {
   chapterContainer = getLinesFromFileIgnoringCommentsAndEmptyLines("data/chapters.txt");
}

void loadLevels() {
   std::vector<std::string> lines = getLinesFromFileIgnoringCommentsAndEmptyLines("data/levels.txt");
   std::stringstream stream;
   levelContainer.reserve(lines.size() / 8); // rough estimate
   Level stack;

   for (const std::string &line: lines) {
      if (line == "[END]") {
         levelContainer.push_back(stack);
         stack = {};
         continue;
      }

      size_t equals = line.find('=');
      if (equals == std::string::npos) {
         printf("WARNING: Malformed line in 'data/levels.txt': '%s'. Expected '=' character.\n", line.c_str());
         continue;
      }

      std::string field = line.substr(0, equals);
      std::string value = line.substr(equals + 1);
      field.erase(field.find_last_not_of(" \n\r\t\v\f") + 1);
      value.erase(0, value.find_first_not_of(" \n\r\t\v\f"));

      if (field == "name") {
         stack.name = value;
      }
      else if (field == "chapter") {
         stack.chapter = value;
      }
      else if (field == "coinTile") {
         // TODO: fix when add tile loading
         // stack.coinTile = value;
      }
      else if (field == "sizeX") {
         stream.clear();
         stream.str(value);
         stream >> stack.sizeX;

         if (stream.rdbuf()->in_avail() != 0) {
            printf("WARNING: Malformed line in 'data/levels.txt': '%s'. Expected number.\n", line.c_str());
         }
      }
      else if (field == "sizeY") {
         stream.clear();
         stream.str(value);
         stream >> stack.sizeY;

         if (stream.rdbuf()->in_avail() != 0) {
            printf("WARNING: Malformed line in 'data/levels.txt': '%s'. Expected number.\n", line.c_str());
         }
      }
      else if (field == "floor" || field == "map") {
         std::vector<std::vector<size_t>> &container = (field == "map" ? stack.map : stack.floor);
         container.reserve(stack.sizeY);
         stream.clear();
         stream.str(value);

         for (size_t y = 0; y < stack.sizeY; ++y) {
            std::vector<size_t> row;
            row.reserve(stack.sizeX);

            for (size_t x = 0; x < stack.sizeX; ++x) {
               size_t value = 0;
               stream >> value;
               row.push_back(value);
            }
            container.push_back(row);
         }

         if (stream.rdbuf()->in_avail() != 0) {
            printf("WARNING: Malformed line in 'data/levels.txt': '%s'. Expected %lu number array. Fields 'sizeX' and 'sizeY' must be defined before fields 'floor' and 'map'.\n", line.c_str(), stack.sizeX * stack.sizeY);
         }
      }
      else {
         printf("WARNING: Malformed line in 'data/levels.txt': '%s'. Unexpected field '%s'.\n", line.c_str(), field.c_str());
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
