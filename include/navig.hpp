#pragma once
#include "render.hpp"
#include <vector>

struct Navigation {
   Navigation();
   ~Navigation();

   void update();
   void render();

   void setIndex(size_t index);
   void resetIndex();

   void addElement(UIElement *element);
   void addElements(const std::vector<UIElement*> &elements);

   UIElement *getElement(size_t index);
   UIElement *getSelectedElement();
   Button *getButton(size_t index);
   Button *getSelectedButton();
   size_t getSelectedIndex();

   bool isIndexValid(size_t index);
   bool anySelected();
   bool selectionChanged();
   bool isSelectedClicked();
   bool isSelectedDown();
   bool isSelectedHoveredOn();

   // Members

   std::vector<UIElement*> elements;   
   size_t index = 0;
   size_t first = 0;
   size_t last = 1;

   bool changed = false;
   bool holding = false;

   float holdTimer = 0.0f;
   float intervalTimer = 0.0f;
};
