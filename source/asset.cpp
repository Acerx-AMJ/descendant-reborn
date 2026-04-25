#include "asset.hpp"
#include <filesystem>
#include <unordered_map>

static std::unordered_map<std::string, Texture> textures;
static std::unordered_map<std::string, Font> fonts;
static std::unordered_map<std::string, Shader> shaders;

Texture &getFallbackTexture() {
   static Texture fallback;
   static bool loaded = false;

   if (!loaded) {
      Image image = GenImageChecked(8, 8, 2, 2, MAGENTA, BLACK);
      fallback = LoadTextureFromImage(image);
      UnloadImage(image);
      loaded = true;
   }
   return fallback;
}

Font &getFallbackFont() {
   static Font fallback = GetFontDefault();
   return fallback;
}

Texture &loadTexture(const std::string &name, const std::string &path) {
   if (textures.count(name)) {
      return textures[name];
   }

   Texture texture = LoadTexture(path.c_str());
   if (texture.id == 0) {
      printf("Failed to load texture from file '%s'.\n", path.c_str());
      return getFallbackTexture();
   }
   textures.insert({name, texture});
   return textures[name];
}

Font &loadFont(const std::string &name, const std::string &path) {
   if (fonts.count(name)) {
      return fonts[name];
   }

   Font font = LoadFontEx(path.c_str(), 120, nullptr, 0);
   SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

   if (font.texture.id == 0) {
      printf("Failed to load font from file '%s'.\n", path.c_str());
      return getFallbackFont();
   }
   fonts.insert({name, font});
   return fonts[name];
}

Shader &loadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath) {
   if (shaders.count(name)) {
      return shaders[name];
   }

   Shader shader = LoadShader(vertexPath.c_str(), fragmentPath.c_str());
   if (shader.id == 0) {
      printf("Failed to load shader from files: vertex: '%s', fragment: '%s'.\n", vertexPath.c_str(), fragmentPath.c_str());
      exit(EXIT_FAILURE);
   }

   shaders.insert({name, shader});
   return shaders[name];
}

void loadTextures() {
   std::filesystem::create_directories("assets/sprites/");
   for (const auto &file: std::filesystem::recursive_directory_iterator("assets/sprites/")) {
      if (file.is_regular_file()) {
         loadTexture(file.path().stem().string(), file.path().string());
      }
   }
}

void loadFonts() {
   std::filesystem::create_directories("assets/fonts/");
   for (const auto &file: std::filesystem::recursive_directory_iterator("assets/fonts/")) {
      if (file.is_regular_file()) {
         loadFont(file.path().stem().string(), file.path().string());
      }
   }
}

void loadShaders() {
   std::filesystem::create_directories("assets/shaders/");
   for (const auto &file: std::filesystem::recursive_directory_iterator("assets/shaders/")) {
      std::string filename = file.path().stem().string();

      if (shaders.count(filename)) {
         continue;
      }

      if (file.path().extension().string() == ".fs") {
         std::filesystem::path vertexPath = "assets/shaders/" + filename + ".vs";
         loadShader(filename, (std::filesystem::exists(vertexPath) ? vertexPath.string() : std::string()), file.path().string());
      } else if (file.path().extension().string() == ".vs") {
         std::filesystem::path fragmentPath = "assets/shaders/" + filename + ".fs";
         loadShader(filename, file.path().string(), (std::filesystem::exists(fragmentPath) ? fragmentPath.string() : std::string()));
      }
   }
}

Texture &getTexture(const std::string &name) {
   if (!textures.count(name)) {
      printf("Texture '%s' does not exist, using fallback texture.\n", name.c_str());
      return getFallbackTexture();
   }
   return textures[name];
}

Font &getFont(const std::string &name) {
   if (!fonts.count(name)) {
      printf("Font '%s' does not exist, using fallback font.\n", name.c_str());
      return getFallbackFont();
   }
   return fonts[name];
}

Shader &getShader(const std::string &name) {
   if (!shaders.count(name)) {
      printf("Shader '%s' does not exist.\n", name.c_str());
      exit(EXIT_FAILURE);
   }
   return shaders[name];
}
