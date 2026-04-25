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
