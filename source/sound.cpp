#include "sound.hpp"
#include "math.hpp"
#include <filesystem>
#include <unordered_map>

constexpr float soundPitchMin = 0.95f;
constexpr float soundPitchMax = 1.05f;

static std::unordered_map<std::string, std::vector<std::string>> savedSounds;
static std::unordered_map<std::string, Sound> sounds;

void playSound(const std::string &name, float volume) {
   if (!savedSounds.count(name) && !sounds.count(name)) {
      printf("Sound '%s' does not exist.\n", name.c_str());
      return;
   }
   Sound sound;

   if (savedSounds.count(name)) {
      std::string &randomName = randomVectorAccess(savedSounds[name]);
      sound = sounds[randomName];
   } else {
      sound = sounds[name];
   }
   SetSoundPitch(sound, randomFloat(soundPitchMin, soundPitchMax));
   SetSoundVolume(sound, volume);
   PlaySound(sound);
}

void loadSound(const std::string &name, const std::string &path) {
   Sound newSound = LoadSound(path.c_str());
   sounds[name] = newSound;
}

void loadSounds() {
   std::filesystem::create_directories("assets/sounds/");
   for (const auto &file: std::filesystem::recursive_directory_iterator("assets/sounds/")) {
      if (!file.is_regular_file()) {
         continue;
      }

      std::string stem = file.path().stem().string();
      loadSound(stem, file.path().string());

      if (isdigit(stem.back())) {
         std::string cleanStem = stem.substr(0, stem.size() - 1);
         savedSounds[cleanStem].push_back(stem);
      }
   }
}
