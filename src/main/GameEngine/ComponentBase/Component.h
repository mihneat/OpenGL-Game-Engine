#pragma once

#include "main/GameEngine/ComponentBase/Transform.h"
#include <iostream>

namespace transform {
    class Transform;
}

namespace component
{
    class Component
    {
    public:
        Component(transform::Transform *transform);
        virtual ~Component();

        virtual void Start() { }
        virtual void Update(const float deltaTime) { }
        virtual void LateUpdate(const float deltaTime) { }
        virtual void InputUpdate(const float deltaTime, const int mods) { }
        virtual void KeyPress(const int key, const int mods) { }
        virtual void KeyRelease(const int key, const int mods) { }
        virtual void MouseMove(const int mouseX, const int mouseY,
            const int deltaX, const int deltaY) { }
        virtual void MouseBtnPress(const int mouseX, const int mouseY,
            const int button, const int mods) { }
        virtual void MouseBtnRelease(const int mouseX, const int mouseY,
            const int button, const int mods) { }
        virtual void MouseScroll(const int mouseX, const int mouseY,
            const int offsetX, const int offsetY) { }
        virtual void WindowResize(int width, int height) { }

        bool GetHasStartActivated();
        void SetHasStartActivated();

        bool IsActive();
        void SetActive(bool active);

        transform::Transform* transform;

    protected:
        bool enabled;
        bool hasStartActivated;

    };
}   // namespace component
