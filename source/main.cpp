#include "loading_state.hpp"
#include <cstdlib>
#include <ctime>
#include <raylib.h>

constexpr int minWindowWidth = 800;
constexpr int minWindowHeight = 600;

int main() {
   srand(time(nullptr));
   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED);
   InitWindow(minWindowWidth, minWindowHeight, "Descendant Reborn");
   SetWindowMinSize(minWindowWidth, minWindowHeight);
   InitAudioDevice();
   SetExitKey(KEY_NULL);

   State *current = new LoadingState();

   while (!WindowShouldClose()) {
      if (current->quittingState) {
         State *newState = current->change();
         delete current;
         current = newState;
      }

      if (!current) {
         break;
      }

      current->updateStateLogic();
      BeginDrawing();
         ClearBackground(BLACK);
         current->render();
         DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, current->alpha));
      EndDrawing();
   }

   CloseWindow();
   CloseAudioDevice();
}
