#include "input.hpp"
#include "sound.hpp"
#include <array>
#include <raylib.h>

enum class KeyState: char {
   none,
   down = 1 << 0,
   pressed = 1 << 1,
   repeated = 1 << 2,
};

enum class MouseState: char {
   none,
   down = 1 << 0,
   pressed = 1 << 1,
};

constexpr KeyState operator | (KeyState a, KeyState b) {
   return static_cast<KeyState>(static_cast<char>(a) | static_cast<char>(b));
}

constexpr bool operator & (KeyState a, KeyState b) {
   return (static_cast<char>(a) & static_cast<char>(b)) != 0;
}

constexpr MouseState operator | (MouseState a, MouseState b) {
   return static_cast<MouseState>(static_cast<char>(a) | static_cast<char>(b));
}

constexpr bool operator & (MouseState a, MouseState b) {
   return (static_cast<char>(a) & static_cast<char>(b)) != 0;
}

static inline std::array<KeyState, KEY_KB_MENU + 1> keys;
static inline std::array<MouseState, MOUSE_BUTTON_BACK + 1> mouse;
static inline bool mouseOnUI = false;
static inline bool shouldBlockInput = false;

void updateInput() {
   mouseOnUI = false;
   keys.fill(KeyState::none);
   mouse.fill(MouseState::none);
}

void setMouseOnUI(bool onUI) {
   if (onUI) {
      mouseOnUI = true;
   }
}

void setInputBlocking(bool block) {
   shouldBlockInput = block;
}

bool isKeyAState(int key, KeyState state) {
   if (shouldBlockInput) return false;
   bool isState = !(keys[key] & state);
   keys[key] = keys[key] | state;
   return isState;
}

bool isKeyDown(int key) {
   return IsKeyDown(key) && isKeyAState(key, KeyState::down);
}

bool isKeyPressed(int key) {
   return IsKeyPressed(key) && isKeyAState(key, KeyState::pressed);
}

bool isKeyRepeated(int key) {
   return (IsKeyPressed(key) && isKeyAState(key, KeyState::pressed))
       || (IsKeyPressedRepeat(key) && isKeyAState(key, KeyState::repeated));
}

bool handleKeyPressWithSound(int key) {
   if (isKeyPressed(key)) {
      playSound("click");
      return true;
   }
   return false;
}

bool isMouseAState(int button, MouseState state) {
   bool isState = !(mouse[button] & state);
   mouse[button] = mouse[button] | state;
   return isState;
}

bool isMouseDownUI(int button) {
   return mouseOnUI && isMouseDown(button);
}

bool isMousePressedUI(int button) {
   return mouseOnUI && isMousePressed(button);
}

bool isMouseDownOutsideUI(int button) {
   return !mouseOnUI && isMouseDown(button);
}

bool isMousePressedOutsideUI(int button) {
   return !mouseOnUI && isMousePressed(button);
}

bool isMouseDown(int button) {
   return IsMouseButtonDown(button) && isMouseAState(button, MouseState::down);
}

bool isMousePressed(int button) {
   return IsMouseButtonPressed(button) && isMouseAState(button, MouseState::pressed);
}
