#pragma once
#include <raylib.h>
#include <string>

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
Vector2 cubicSize(float size);

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
void drawTexture(Texture texture, Vector2 position, Vector2 size, Color color, float rotation = 0.0f);
void drawTextureCentered(Texture texture, Vector2 position, Vector2 size, Color color, float rotation = 0.0f);

// ui module

struct UIElement {
   virtual ~UIElement() = default;
   virtual void update(bool navigHovering = false, bool navigDown = false, bool navigClicked = false) = 0;
   virtual void render() = 0;

   bool hovering = false;
   bool down = false;
   bool clicked = false;
};

struct Button: public UIElement {
   static Button *make(Texture texture, Font font, const std::string &text, float fontSize);
   void init(Texture texture, Font font, const std::string &text, float fontSize);

   void update(bool navigHovering = false, bool navigDown = false, bool navigClicked = false) override;
   void render() override;

   Vector2 position;
   Vector2 size;
   Texture texture;
   Font font;

   std::string text;
   size_t ID = 0;

   bool disabled = false;
   float scale = 1.0f;
   float fontSize = 0.0f;
};
