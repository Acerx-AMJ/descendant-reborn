#pragma once

void updateInput();
void setMouseOnUI(bool onUI);
void setInputBlocking(bool block);

bool isKeyDown(int key);
bool isKeyPressed(int key);
bool isKeyRepeated(int key);
bool handleKeyPressWithSound(int key);

bool isMouseDownUI(int button);
bool isMousePressedUI(int button);
bool isMouseDownOutsideUI(int button);
bool isMousePressedOutsideUI(int button);
bool isMouseDown(int button);
bool isMousePressed(int button);
