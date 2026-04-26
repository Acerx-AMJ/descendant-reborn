#include "navig.hpp"

constexpr float holdDelay = 0.4f;
constexpr float holdInterval = 0.1f;

Navigation::Navigation() {
   elements.push_back(nullptr); // 0 is null element
}

Navigation::~Navigation() {
   for (size_t i = 1; i < elements.size(); ++i) {
      destroy(elements[i]);
   }
}

void Navigation::update() {
   size_t previous = index;

   bool shouldGoUp = IsKeyPressed(KEY_UP);
   bool shouldGoDown = IsKeyPressed(KEY_DOWN);

   if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
      holdTimer += GetFrameTime();

      if (!holding) {
         holdTimer = 0.0f;
         holding = true;
      }
      else if (holdTimer >= holdDelay) {
         intervalTimer += GetFrameTime();

         if (intervalTimer >= holdInterval) {
            intervalTimer -= holdInterval;
            if (IsKeyDown(KEY_UP)) shouldGoUp = true;
            if (IsKeyDown(KEY_DOWN)) shouldGoDown = true;
         }
      }
   }
   else {
      holding = false;
   }

   if (shouldGoUp) {
      index = (index == 0 ? elements.size() - 1 : index - 1);
   }

   if (shouldGoDown) {
      index = (index + 1) % elements.size();
   }

   changed = previous != index;
   for (size_t i = 1; i < elements.size(); ++i) {
      if (index != i) {
         elements[i]->update();
         continue;
      }

      elements[i]->update(true, IsKeyDown(KEY_ENTER), IsKeyPressed(KEY_ENTER));
   }
}

void Navigation::render() {
   for (size_t i = 1; i < elements.size(); ++i) {
      elements[i]->render();
   }
}

void Navigation::setIndex(size_t index) {
   this->index = index;
}

void Navigation::resetIndex() {
   index = 0;
}

void Navigation::addElement(UIElement *element) {
   elements.push_back(element);
}

void Navigation::addElements(const std::vector<UIElement*> &elements) {
   this->elements.reserve(this->elements.size() + elements.size());
   for (UIElement *element: elements) {
      this->elements.push_back(element);
   }
}

UIElement *Navigation::getElement(size_t index) {
   return isIndexValid(index) ? elements[index] : nullptr;
}

UIElement *Navigation::getSelectedElement() {
   return getElement(index);
}

Button *Navigation::getButton(size_t index) {
   return isIndexValid(index) ? (Button*)elements[index] : nullptr;
}

Button *Navigation::getSelectedButton() {
   return getButton(index);
}

size_t Navigation::getSelectedIndex() {
   return index;
}

bool Navigation::isIndexValid(size_t index) {
   return index != 0 && index < elements.size();
}

bool Navigation::anySelected() {
   return isIndexValid(index);
}

bool Navigation::selectionChanged() {
   return changed;
}

bool Navigation::isSelectedClicked() {
   return anySelected() && getSelectedElement()->clicked;
}

bool Navigation::isSelectedDown() {
   return anySelected() && getSelectedElement()->down;
}

bool Navigation::isSelectedHoveredOn() {
   return anySelected() && getSelectedElement()->hovering;
}
