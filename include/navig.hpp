#pragma once
#include "render.hpp"
#include <vector>

constexpr size_t invalidNavigIndex = -1; // overflow
constexpr float holdDelay = 0.4f;
constexpr float holdInterval = 0.1f;

struct NavigElement {
   UIElement *element = nullptr;
   size_t up = 0;
   size_t left = 0;
   size_t right = 0;
   size_t down = 0;
};

struct KeyState {
   int key = 0;
   bool pressed = false;
   bool holding = false;
   float holdTimer = 0.0f;
   float intervalTimer = 0.0f;
};

struct Navigation {
   Navigation();
   ~Navigation();

   void update();
   void updateKey(KeyState &key);
   void render();

   void addElement(UIElement *element, size_t up, size_t left, size_t right, size_t down);
   void addElements(const std::vector<UIElement*> &elements);

   void setNoSelectionNavig(size_t up, size_t down);
   void setNoSelectionNavigDefault(size_t def);

   UIElement *getElement(size_t index);
   UIElement *getSelectedElement();

   template<class T>
   T *get(size_t index) {
      return (T*)getElement(index);
   }

   template<class T>
   T *getSelected() {
      return (T*)getSelectedElement();
   }

   bool isIndexValid(size_t index);
   bool anySelected();

   // Members

   std::vector<NavigElement> elements;   
   KeyState up, left, right, down, tab;
   size_t index = 0;
};
