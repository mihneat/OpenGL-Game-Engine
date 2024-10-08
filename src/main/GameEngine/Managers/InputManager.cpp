﻿#include "InputManager.h"

WindowObject* InputManager::window;

// Search for "GLFW_KEY_" in glfw3.h to see all possible keys 
bool InputManager::KeyHold(int keyCode)
{
    if (window == nullptr)
        throw std::exception("InputManager not initialized!");
    
    return window->KeyHold(keyCode);
}

bool InputManager::MouseHold(int keyCode)
{
    if (window == nullptr)
        throw std::exception("InputManager not initialized!");
    
    return window->MouseHold(keyCode);
}

void InputManager::ShowCursor(bool visible)
{
    if (visible)
        window->ShowPointer();
    else
        window->DisablePointer();
}
