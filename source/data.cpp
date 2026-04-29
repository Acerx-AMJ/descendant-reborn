#include "data.hpp"
#include "file.hpp"
#include <fstream>
#include <sstream>

static std::vector<std::string> playerIconContainer;
static std::vector<Vector3> playerColorContainer;

// game data module

void loadData() {
   loadPlayerIcons();
   loadPlayerColors();
}

void loadPlayerIcons() {
   std::vector<std::string> lines = getLinesFromFileIgnoringCommentsAndEmptyLines("data/icons.txt");
   playerIconContainer = lines;
}

void loadPlayerColors() {
   std::vector<std::string> lines = getLinesFromFileIgnoringCommentsAndEmptyLines("data/colors.txt");
   std::stringstream stream;
   playerColorContainer.reserve(lines.size());

   for (const std::string &line: lines) {
      stream.clear();
      stream.str(line);
      std::string stringR, stringG, stringB;
      stream >> stringR >> stringG >> stringB;

      // malformed input, excess characters
      if (stream.rdbuf()->in_avail() != 0) {
         printf("WARNING: Malformed line in 'data/colors.txt': '%s'. %ld excess characters, expected 3 real numbers.\n", line.c_str(), stream.rdbuf()->in_avail());
         continue;
      }

      try {
         Vector3 color;
         color.x = std::stof(stringR);
         color.y = std::stof(stringG);
         color.z = std::stof(stringB);
         playerColorContainer.push_back(color);
      }
      // strings are not numbers, just ignore it
      catch (...) {
         printf("WARNING: Malformed line in 'data/colors.txt': '%s'. Expected 3 real numbers.\n", line.c_str());
         continue;
      }
   }
}

size_t getPlayerIconCount() {
   return playerIconContainer.size();
}

std::string getPlayerIcon(size_t ID) {
   if (ID >= getPlayerIconCount()) {
      return {};
   }
   return playerIconContainer[ID];
}

std::vector<std::string> &getPlayerIconContainer() {
   return playerIconContainer;
}

size_t getPlayerColorCount() {
   return playerColorContainer.size();
}

Vector3 getPlayerColor(size_t ID) {
   if (ID >= getPlayerColorCount()) {
      return {0, 0, 0};
   }
   return playerColorContainer[ID];
}

std::vector<Vector3> &getPlayerColorContainer() {
   return playerColorContainer;
}

// player data module

static CustomizationData customizationData;

CustomizationData getCustomizationData() {
   return customizationData;
}

CustomizationData loadCustomizationData() {
   CustomizationData data, loaded;
   std::fstream file ("data/cd.data", std::ios::in);
   if (!file.is_open()) {
      return data; // default data
   }
   
   file.read(reinterpret_cast<char*>(&loaded), sizeof(loaded));

   if (data.magicNumber != loaded.magicNumber) {
      return data; // default data
   }
   return loaded;
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
