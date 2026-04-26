#include "loading_state.hpp"
#include "asset.hpp"
#include "data.hpp"
#include "file.hpp"
#include "menu_state.hpp"
#include "render.hpp"
#include "sound.hpp"


LoadingState::LoadingState() {
   loadingText = "Loading Fonts... ";
   loadFont("slackey", "assets/fonts/slackey.ttf");
   loadTexture("loading", "assets/sprites/loading.png");
   splash = getRandomLineFromFile("assets/splash.txt");
   splash = wrap(splash, getFont("slackey"), GetScreenWidth() - 50.0f * getWidthRatio(), getFontSize(50.0f), getFontSize(1.0f));
}

void LoadingState::update() {
   iconRotation += GetFrameTime() * 360.0f;

   if (phase == Load::fonts) {
      loadFonts();
      loadingText = "Loading Textures... ";
      phase = Load::textures;
   }
   else if (phase == Load::textures) {
      loadTextures();
      loadingText = "Loading Shaders... ";
      phase = Load::shaders;
   }
   else if (phase == Load::shaders) {
      loadShaders();
      loadingText = "Loading Sounds... ";
      phase = Load::sounds;
   }
   else if (phase == Load::sounds) {
      loadSounds();
      loadingText = "Loading Game Data... ";
      phase = Load::data;
   }
   else if (phase == Load::data) {
      loadData();
      loadingText = "Loading Done!";
      phase = Load::count;
   }
   else if (phase == Load::count) {
      finalWaitTimer += GetFrameTime();
      fadingOut = (finalWaitTimer >= 1.0f);
   }
}

void LoadingState::render() {
   std::string finalLoadingText = loadingText;
   if (phase != Load::count) {
      finalLoadingText = loadingText + std::to_string(int(phase)) + "/" + std::to_string(int(Load::count));
   }

   Font font = getFont("slackey");
   Texture icon = getTexture("loading");

   drawTextCentered(font, getScreenCenterOffset({0.0f, getHeightRatio() * -175.0f}), finalLoadingText.c_str(), 80, WHITE);
   drawTextCentered(font, getScreenCenterOffset({0.0f, getHeightRatio() * 100.0f}), splash.c_str(), 40, WHITE);
   drawTextureCentered(icon, getScreenCenter(), cubicSize(70.0f), WHITE, iconRotation);
}

void LoadingState::fixedUpdate() {

}

void LoadingState::updateResponsiveness() {

}

State *LoadingState::change() {
   return new MenuState();
}
