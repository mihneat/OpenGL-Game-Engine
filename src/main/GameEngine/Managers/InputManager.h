#pragma once
#include "core/window/window_object.h"
#include "main/GameEngine/GameEngine.h"

class InputManager
{
public:
    static bool KeyHold(int keyCode);

private:
    static WindowObject* window;

    friend class m1::GameEngine;
};
