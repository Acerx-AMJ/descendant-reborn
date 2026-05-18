#include "navig.hpp"
#include <cmath>

Navigation::Navigation() {
   up.key = KEY_UP;
   left.key = KEY_LEFT;
   right.key = KEY_RIGHT;
   down.key = KEY_DOWN;
   tab.key = KEY_TAB;

   elements.push_back({}); // null
}

Navigation::~Navigation() {
   for (NavigElement &element: elements) {
      if (element.element) {
         destroy(element.element);
      }
   }
}

void Navigation::update() {
   updateKey(up);
   updateKey(left);
   updateKey(right);
   updateKey(down);
   updateKey(tab);

   if (up.pressed && elements[index].up < elements.size()) {
      index = elements[index].up;
   }

   if (down.pressed && elements[index].down < elements.size()) {
      index = elements[index].down;
   }

   if (left.pressed && elements[index].left < elements.size()) {
      index = elements[index].left;
   }

   if (right.pressed && elements[index].right < elements.size()) {
      index = elements[index].right;
   }

   if (tab.pressed && (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))) {
      index = (index == 0 ? elements.size() - 1 : index - 1);
   }

   if (tab.pressed && !IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_RIGHT_SHIFT)) {
      index = (index + 1) % elements.size();
   }

   for (size_t i = 0; i < elements.size(); ++i) {
      if (!elements[i].element) {
         continue;
      }

      if (index != i) {
         elements[i].element->update();
         continue;
      }

      elements[i].element->update(true, IsKeyDown(KEY_ENTER), IsKeyPressed(KEY_ENTER));
   }
}

void Navigation::updateKey(KeyState &key) {
   key.pressed = IsKeyPressed(key.key);

   if (IsKeyDown(key.key)) {
      if (!key.holding) {
         key.intervalTimer = 0.0f;
         key.holdTimer = 0.0f;
         key.holding = true;
      }

      key.holdTimer += GetFrameTime();
      if (key.holdTimer >= holdDelay) {
         key.intervalTimer += GetFrameTime();
      }

      if (key.intervalTimer >= holdInterval) {
         key.intervalTimer -= holdInterval;
         key.pressed = true;
      }
   }
   else {
      key.holding = false;
   }
}

void Navigation::render() {
   for (NavigElement &element: elements) {
      if (element.element) {
         element.element->render();
      }
   }
}

void Navigation::addElement(UIElement *element, size_t up, size_t left, size_t right, size_t down) {
   NavigElement instance = {element->copy(), up, left, right, down};
   elements.push_back(instance);
}

void Navigation::addElements(const std::vector<UIElement*> &elements) {
   this->elements.reserve(elements.size() + 1);

   for (size_t i = 0; i < elements.size(); ++i) {
      NavigElement instance = {elements[i]->copy(), i, invalidNavigIndex, invalidNavigIndex, (i + 2) % (elements.size() + 1)};
      this->elements.push_back(instance);
   }
   setNoSelectionNavig(this->elements.size() - 1, fmin(1, this->elements.size()));
}

void Navigation::setNoSelectionNavig(size_t up, size_t down) {
   elements[0].up = up;
   elements[0].down = down;
}

void Navigation::setNoSelectionNavigDefault(size_t def) {
   elements[0].up = elements[0].down = elements[0].left = elements[0].right = def;
}

UIElement *Navigation::getElement(size_t index) {
   return isIndexValid(index) ? elements[index].element : nullptr;
}

UIElement *Navigation::getSelectedElement() {
   return getElement(index);
}

bool Navigation::isIndexValid(size_t index) {
   return index != 0 && index < elements.size();
}

bool Navigation::anySelected() {
   return isIndexValid(index);
}
