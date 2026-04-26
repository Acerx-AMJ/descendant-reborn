#include "render.hpp"
#include <raymath.h>

// math module

float getWidthRatio() {
   return GetScreenWidth() / 1920.0f;
}

float getHeightRatio() {
   return GetScreenHeight() / 1080.0f;
}

float getCubicRatio() {
   return fminf(getWidthRatio(), getHeightRatio());
}

float getFontSize(float fontSize) {
   return fontSize * getCubicRatio();
}

Vector2 getScreenSize() {
   return {GetScreenWidth() * 1.0f, GetScreenHeight() * 1.0f};
}

Vector2 getScreenCenter() {
   return {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
}

Vector2 getScreenCenterOffset(Vector2 offset) {
   return Vector2Add(getScreenCenter(), offset);
}

Vector2 translateRatioToScreen(Vector2 ratio) {
   return Vector2Multiply(getScreenSize(), ratio);
}

Vector2 translateScreenToRatio(Vector2 screen) {
   return Vector2Divide(screen, getScreenSize());
}

Vector2 cubicSize(float size) {
   return {getCubicRatio() * size, getCubicRatio() * size};
}

Vector2 getTextSize(Font font, const char *text, float fontSize, float spacing) {
   return MeasureTextEx(font, text, fontSize, spacing);
}

Vector2 getTextOrigin(Font font, const char *text, float fontSize, float spacing) {
   return getOrigin(MeasureTextEx(font, text, fontSize, spacing));
}

Vector2 getOrigin(Vector2 size) {
   return Vector2Scale(size, 0.5f);
}

Rectangle getSource(Texture texture) {
   return {0.0f, 0.0f, texture.width * 1.0f, texture.height * 1.0f};
}

Rectangle getRectangle(Vector2 position, Vector2 size) {
   return {position.x, position.y, size.x, size.y};
}

// render module

void drawText(Font font, Vector2 position, const char *text, float fontSize, Color color) {
   DrawTextPro(font, text, position, {0.0f, 0.0f}, 0.0f, getFontSize(fontSize), getFontSize(1.0f), color);
}

void drawTextCentered(Font font, Vector2 position, const char *text, float fontSize, Color color) {
   float fontSizeScaled = getFontSize(fontSize);
   float spacingScaled = getFontSize(1.0f);
   DrawTextPro(font, text, position, getTextOrigin(font, text, fontSizeScaled, spacingScaled), 0.0f, fontSizeScaled, spacingScaled, color);
}

void drawTextRatio(Font font, Vector2 ratio, const char *text, float fontSize, Color color) {
   DrawTextPro(font, text, translateRatioToScreen(ratio), {0.0f, 0.0f}, 0.0f, getFontSize(fontSize), getFontSize(1.0f), color);
}

void drawTextRatioCentered(Font font, Vector2 ratio, const char *text, float fontSize, Color color) {
   float fontSizeScaled = getFontSize(fontSize);
   float spacingScaled = getFontSize(1.0f);
   DrawTextPro(font, text, translateRatioToScreen(ratio), getTextOrigin(font, text, fontSizeScaled, spacingScaled), 0.0f, fontSizeScaled, spacingScaled, color);
}

void drawTexture(Texture texture, Vector2 position, Vector2 size, Color color, float rotation) {
   DrawTexturePro(texture, getSource(texture), getRectangle(position, size), {0.0f, 0.0f}, rotation, color);
}

void drawTextureCentered(Texture texture, Vector2 position, Vector2 size, Color color, float rotation) {
   DrawTexturePro(texture, getSource(texture), getRectangle(position, size), getOrigin(size), rotation, color);
}

// ui module

constexpr float buttonScaleMin = 0.97f;
constexpr float buttonScaleMax = 1.03f;
constexpr Color buttonDisabledColor = {170, 170, 150, 255};

Button *Button::make(Texture texture, Font font, const std::string &text, float fontSize) {
   Button *button = new Button();
   button->init(texture, font, text, fontSize);
   return button;
}

void Button::init(Texture texture, Font font, const std::string &text, float fontSize) {
   this->texture = texture;
   this->font = font;
   this->text = text;
   this->fontSize = fontSize;
}

void Button::update(bool navigHovering, bool navigDown, bool navigClicked) {
   Vector2 mouse = GetMousePosition();
   Vector2 scaledSize = Vector2Scale(size, getCubicRatio());
   Vector2 realPosition = Vector2Subtract(position, getOrigin(scaledSize));
   bool actuallyHovering = CheckCollisionPointRec(mouse, getRectangle(realPosition, scaledSize));
   hovering = actuallyHovering || navigHovering;

   if (disabled) {
      down = false;
      clicked = false;
   }
   else {
      down = navigDown || (actuallyHovering && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
      clicked = navigClicked || (actuallyHovering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
   }

   float dt = GetFrameTime();
   if (down) {
      scale = fmaxf(scale - dt, buttonScaleMin);
   }
   else if (hovering) {
      scale = fminf(scale + dt, buttonScaleMax);
   }
   else if (scale < 1.0f) {
      scale = fminf(scale + dt, 1.0f);
   }
   else if (scale > 1.0f) {
      scale = fmaxf(scale - dt, 1.0f);
   }
}

void Button::render() {
   Color tint = disabled ? buttonDisabledColor : WHITE;
   drawTextureCentered(texture, position, Vector2Scale(size, scale * getCubicRatio()), tint);

   if (!text.empty()) {
      drawTextCentered(font, position, text.c_str(), fontSize * scale, tint);
   }
}

UIElement *Button::copy() {
   refCount += 1;
   return this;
}

void destroy(UIElement *element) {
   element->refCount -= 1;
   if (element->refCount == 0) {
      delete element;
   }
}
