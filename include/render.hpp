#pragma once
#include <raylib.h>
#include <string>

// math module

Vector2 Vector2DivideValue(Vector2 vector, float value);

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
   virtual void update(bool navigHovering = false, bool navigDown = false, bool navigClicked = false);
   virtual void render() = 0;
   UIElement *copy();

   Vector2 position;
   Vector2 size;

   size_t ID = 0;
   size_t refCount = 1;

   float scale = 1.0f;
   float scaleMin = 0.97f;
   float scaleMax = 1.03f;
   bool hovering = false;
   bool down = false;
   bool clicked = false;
   bool disabled = false;

protected:

   void updateInternalState(bool navigHovering, bool navigDown, bool navigClicked);
};

struct Button: public UIElement {
   static Button *make(Texture texture, Vector2 size, Font font, const std::string &text, float fontSize);
   static Button *make(Texture texture, Vector2 size);
   
   void init(Texture texture, Vector2 size, Font font, const std::string &text, float fontSize);
   void init(Texture texture, Vector2 size);
   void render() override;

   Font font;
   Texture texture;
   std::string text;
   float fontSize = 0.0f;
};

struct TextureRect: public UIElement {
   static TextureRect *make(Texture texture, Vector2 size);
   static TextureRect *make(Color color, Vector2 size);
   
   void init(Texture texture, Vector2 size);
   void init(Color color, Vector2 size);
   void render() override;

   Texture texture;
   Color color;
};

struct TextInput: public UIElement {
   static TextInput *make(Texture texture, Vector2 size, Font font, const std::string &fallback, size_t maxChars, float fontSize);
   void init(Texture texture, Vector2 size, Font font, const std::string &fallback, size_t maxChars, float fontSize);

   void update(bool navigHovering = false, bool navigDown = false, bool navigClicked = false) override;
   void render() override;

   Font font;
   Texture texture;
   std::string text, fallback;
   size_t maxChars = (size_t)-1; // explicit overflow
   float fontSize = 0.0f;
   bool changed = false;
   bool active = false;
};

void destroy(UIElement *element);
