#pragma once

#include "main/GameEngine/ComponentBase/Transform.h"

// Check 'Component.h' for more details
#define SERIALIZE_CLASS

// Check 'Component.h' for all serializable fields
#define SERIALIZE_FIELD

// Check 'Component.h' for more details
#define MARK_SERIALIZABLE friend class Serializer;

/**
 * Serialized field types:
 *
 * NO const or unsigned
 *
 * bool <name>;
 * int <name>;
 * float <name>;
 * glm::vec2 <name>;
 * glm::vec3 <name>;
 * glm::vec4 <name>;
 * Transform* <name>;
 * 
 */

struct SerializedField;

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

        virtual void Awake() { }
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
        
        bool GetHasAwakeActivated();
        void SetHasAwakeActivated();
        bool GetHasStartActivated();
        void SetHasStartActivated();

        bool IsActive();
        void SetActive(bool active);

        transform::Transform* transform;

    protected:
        bool enabled;
        bool hasAwakeActivated;
        bool hasStartActivated;

        friend class transform::Transform;
    };
}   // namespace component
