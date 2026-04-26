#include "file.hpp"
#include "math.hpp"
#include <fstream>
#include <vector>
#include <sstream>

std::vector<std::string> getLinesFromFileIgnoringCommentsAndEmptyLines(const std::string &path) {
   std::fstream file (path);
   if (!file.is_open()) {
      return {};
   }

   std::vector<std::string> lines;
   lines.reserve(64);
   std::string line;

   while (std::getline(file, line)) {
      if (size_t pos = line.find_first_of('#'); pos != std::string::npos) {
         line.erase(pos);
      }
      line.erase(line.find_last_not_of(" \n\r\t\v\f") + 1);
      line.erase(0, line.find_first_not_of(" \n\r\t\v\f"));

      if (!line.empty()) {
         lines.push_back(line);
      }
   }
   return lines;
}

std::string getRandomLineFromFile(const std::string &path) {
   std::fstream file (path);
   if (!file.is_open()) {
      return "";
   }

   std::vector<std::string> lines;
   lines.reserve(64);
   std::string line;

   while (std::getline(file, line)) {
      lines.push_back(line);
   }
   return randomVectorAccess(lines);
}

std::string wrap(const std::string &string, Font font, float maxWidth, float fontSize, float spacing) {
   auto wrap = [font, maxWidth, fontSize, spacing](const std::string &s) -> bool {
      return MeasureTextEx(font, s.c_str(), fontSize, spacing).x > maxWidth;
   };

   if (!wrap(string)) {
      return string;
   }

   std::string original = string;
   std::string_view split = original; // string cannot be viewed because it gets changed
   std::stringstream result;

   while (!split.empty()) {
      std::string current (split);
      if (!wrap(current)) {
         result << split;
         break;
      }

      size_t left = 0, right = split.size();
      while (left < right) {
         size_t mid = (left + right) / 2;
         std::string temp = std::string(split.substr(0, mid)) + "-";

         if (wrap(temp)) {
            right = mid;
         } else {
            left = mid + 1;
         }
      }

      size_t cut = left > 0 ? left - 1 : 0;
      bool punctuation = (cut < split.size() && std::ispunct(split[cut]));
      if (punctuation) cut += 1;

      std::string_view truncated = split.substr(0, cut);
      std::string_view remainder = split.substr(cut);

      if (!remainder.empty() && std::isspace(remainder.front())) {
         remainder = remainder.substr(1);
      }

      bool dash = !truncated.empty() && !remainder.empty() && std::isalpha(truncated.back()) && std::isalpha(split.front());
      result << truncated << (dash ? "-\n" : "\n");
      split = remainder;
   }
   return result.str();
}
