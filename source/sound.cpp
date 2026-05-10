#include "sound.hpp"
#include "math.hpp"
#include <filesystem>
#include <unordered_map>

constexpr float soundPitchMin = 0.95f;
constexpr float soundPitchMax = 1.05f;

static std::unordered_map<std::string, std::vector<Sound>> sounds;

void playSound(const std::string &name, float volume) {
   if (!sounds.count(name)) {
      printf("Sound '%s' does not exist.\n", name.c_str());
      return;
   }

   Sound sound = randomVectorAccess(sounds[name]);
   SetSoundPitch(sound, randomFloat(soundPitchMin, soundPitchMax));
   SetSoundVolume(sound, volume);
   PlaySound(sound);
}

void loadSound(const std::string &name, const std::string &path) {
   Sound newSound = LoadSound(path.c_str());
   sounds[name].push_back(newSound);
}

void loadSounds() {
   std::filesystem::create_directories("assets/sounds/");
   for (const auto &file: std::filesystem::recursive_directory_iterator("assets/sounds/")) {
      if (!file.is_regular_file()) {
         continue;
      }

      std::string stem = file.path().stem().string();
      if (isdigit(stem.back())) {
         stem = stem.substr(0, stem.find_last_not_of("1234567890") + 1);
      }
      loadSound(stem, file.path().string());
   }
}
