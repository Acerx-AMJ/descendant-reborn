#pragma once
#include <raylib.h>

// math module

float getWidthRatio();
float getHeightRatio();
float getCubicRatio();
float getFontSize(float fontSize);

Vector2 getScreenSize();
Vector2 getScreenCenter();
Vector2 getScreenCenterOffset(Vector2 offset);
Vector2 translateRatioToScreen(Vector2 ratio);
Vector2 translateScreenToRatio(Vector2 screen);

Vector2 getTextSize(Font font, const char *text, float fontSize, float spacing);
Vector2 getTextOrigin(Font font, const char *text, float fontSize, float spacing);
Vector2 getOrigin(Vector2 size);

Rectangle getSource(Texture texture);
Rectangle getRectangle(Vector2 position, Vector2 size);

// render module

void drawText(Font font, Vector2 position, const char *text, float fontSize, Color color);
void drawTextCentered(Font font, Vector2 position, const char *text, float fontSize, Color color);
void drawTextRatio(Font font, Vector2 ratio, const char *text, float fontSize, Color color);
void drawTextRatioCentered(Font font, Vector2 ratio, const char *text, float fontSize, Color color);
