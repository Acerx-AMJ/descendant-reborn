#include "render.hpp"
#include "input.hpp"
#include "sound.hpp"
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

void drawTextSemiCentered(Font font, Vector2 position, const char *text, float fontSize, Color color) {
   float fontSizeScaled = getFontSize(fontSize);
   float spacingScaled = getFontSize(1.0f);
   DrawTextPro(font, text, position, {0.0f, getTextOrigin(font, text, fontSizeScaled, spacingScaled).y}, 0.0f, fontSizeScaled, spacingScaled, color);
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

void UIElement::update(bool navigHovering, bool navigDown, bool navigClicked) {
   updateInternalState(navigHovering, navigDown, navigClicked);
}

void UIElement::updateInternalState(bool navigHovering, bool navigDown, bool navigClicked) {
   Vector2 mouse = GetMousePosition();
   Vector2 scaledSize = Vector2Scale(size, getCubicRatio());
   Vector2 realPosition = Vector2Subtract(position, getOrigin(scaledSize));
   bool actuallyHovering = CheckCollisionPointRec(mouse, getRectangle(realPosition, scaledSize));
   bool wasHovering = hovering;

   hovering = actuallyHovering || navigHovering;
   down = navigDown || (actuallyHovering && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
   clicked = navigClicked || (actuallyHovering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
   setMouseOnUI(actuallyHovering);

   float dt = GetFrameTime();
   if (down) {
      scale = fmaxf(scale - dt, scaleMin);
   }
   else if (hovering) {
      scale = fminf(scale + dt, scaleMax);
   }
   else if (scale < 1.0f) {
      scale = fminf(scale + dt, 1.0f);
   }
   else if (scale > 1.0f) {
      scale = fmaxf(scale - dt, 1.0f);
   }

   if (!wasHovering && hovering) {
      playSound("hover");
   }

   if (clicked) {
      playSound("click");
   }
}

UIElement *UIElement::copy() {
   refCount += 1;
   return this;
}

Button *Button::make(Texture texture, Vector2 size, Font font, const std::string &text, float fontSize) {
   Button *button = new Button();
   button->init(texture, size, font, text, fontSize);
   return button;
}

Button *Button::make(Texture texture, Vector2 size) {
   Button *button = new Button();
   button->init(texture, size);
   return button;
}

void Button::init(Texture texture, Vector2 size, Font font, const std::string &text, float fontSize) {
   this->texture = texture;
   this->size = size;
   this->font = font;
   this->text = text;
   this->fontSize = fontSize;
}

void Button::init(Texture texture, Vector2 size) {
   this->texture = texture;
   this->size = size;
   scaleMin = 0.92f;
   scaleMax = 1.08f;
}

void Button::render() {
   drawTextureCentered(texture, position, Vector2Scale(size, scale * getCubicRatio()), WHITE);

   if (!text.empty()) {
      drawTextCentered(font, position, text.c_str(), fontSize * scale, WHITE);
   }
}

Text *Text::make(Font font, const std::string &text, float fontSize) {
   Text *element = new Text();
   element->init(font, text, fontSize);
   return element;
}

void Text::init(Font font, const std::string &text, float fontSize) {
   this->font = font;
   this->text = text;
   this->fontSize = fontSize;
}

void Text::update(bool navigHovering, bool navigDown, bool navigClicked) {
   size = getTextSize(font, text.c_str(), getFontSize(fontSize), getFontSize(1.0f));
   position.x += size.x / 2.0f;
   updateInternalState(navigHovering, navigDown, navigClicked);
   position.x -= size.x / 2.0f;

   Color target = hovering ? WHITE : fadedTextColor;
   color = ColorLerp(color, target, GetFrameTime() * 5.0f);
}

void Text::render() {
   drawTextSemiCentered(font, position, text.c_str(), fontSize * scale, color);
}

TextureRect *TextureRect::make(Texture texture, Vector2 size) {
   TextureRect *rect = new TextureRect();
   rect->init(texture, size);
   return rect;
}

TextureRect *TextureRect::make(Color color, Vector2 size) {
   TextureRect *rect = new TextureRect();
   rect->init(color, size);
   return rect;
}

void TextureRect::init(Texture texture, Vector2 size) {
   this->texture = texture;
   this->size = size;
   scaleMin = 0.92f;
   scaleMax = 1.08f;
}

void TextureRect::init(Color color, Vector2 size) {
   this->texture.id = 0;
   this->color = color;
   this->size = size;
   scaleMin = 0.92f;
   scaleMax = 1.08f;
}

void TextureRect::render() {
   if (texture.id == 0) {
      Vector2 sizeScaled = Vector2Scale(size, scale * getCubicRatio());
      DrawRectanglePro(getRectangle(position, sizeScaled), getOrigin(sizeScaled), 0, color);
      return;
   }

   drawTextureCentered(texture, position, Vector2Scale(size, scale * getCubicRatio()), WHITE);
}

TextInput *TextInput::make(Texture texture, Vector2 size, Font font, const std::string &fallback, size_t maxChars, float fontSize) {
   TextInput *input = new TextInput();
   input->init(texture, size, font, fallback, maxChars, fontSize);
   return input;
}

void TextInput::init(Texture texture, Vector2 size, Font font, const std::string &fallback, size_t maxChars, float fontSize) {
   this->texture = texture;
   this->size = size;
   this->font = font;
   this->fallback = fallback;
   this->maxChars = maxChars;
   this->fontSize = fontSize;
}

void TextInput::update(bool navigHovering, bool navigDown, bool navigClicked) {
   Vector2 mouse = GetMousePosition();
   Vector2 scaledSize = Vector2Scale(size, getCubicRatio());
   Vector2 realPosition = Vector2Subtract(position, getOrigin(scaledSize));
   bool actuallyHovering = CheckCollisionPointRec(mouse, getRectangle(realPosition, scaledSize));
   bool wasHovering = hovering;

   hovering = actuallyHovering || navigHovering;
   down = navigDown || (actuallyHovering && IsMouseButtonDown(MOUSE_BUTTON_LEFT));
   clicked = navigClicked || (actuallyHovering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
   setMouseOnUI(actuallyHovering);

   float dt = GetFrameTime();
   if (active) {
      scale = fminf(scale + dt, scaleMax);
   }
   else {
      scale = fmaxf(scale - dt, 1.0f);
   }
   
   if (!wasHovering && hovering) {
      playSound("hover");
   }

   if (clicked) {
      playSound("click");
   }

   setInputBlocking(false);
   if (clicked) {
      active = !active;
   }
   else if (active && ((!actuallyHovering && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      || isKeyPressed(KEY_ENTER) || isKeyPressed(KEY_ESCAPE))) {
      active = false;
   }

   changed = false;

   if (!active) {
      return;
   }

   if (!text.empty() && (isKeyRepeated(KEY_BACKSPACE) || isKeyRepeated(KEY_DELETE))) {
      if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
         do {
            text.pop_back();
         } while (!text.empty() && !isspace(text.back()));
      }
      else {
         text.pop_back();
      }
      changed = true;
   }

   for (char c = GetCharPressed(); c != 0 && text.size() < maxChars; c = GetCharPressed()) {
      text.push_back(c);
      changed = true;
   }
   setInputBlocking(true);
}

void TextInput::render() {
   drawTextureCentered(texture, position, Vector2Scale(size, scale * getCubicRatio()), WHITE);

   Color tint = WHITE;
   if (active) {
      unsigned char value = std::sin(GetTime() * 20.0f) * 55.0f + 200.0f;
      tint = {value, value, value, 255};
   }

   std::string displayText = text.empty() ? fallback : text;
   drawTextCentered(font, position, displayText.c_str(), fontSize * scale, tint);
}

void destroy(UIElement *element) {
   element->refCount -= 1;
   if (element->refCount == 0) {
      delete element;
   }
}
