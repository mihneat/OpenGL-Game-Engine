#pragma once

#include <string>

#include "main/GameEngine/ComponentBase/Transform.h"

// Check 'Component.h' for more details
#define SERIALIZE_CLASS

// Check 'Component.h' for all serializable fields
#define SERIALIZE_FIELD

// Check 'Component.h' for exact structure
#define SERIALIZE_ENUM

// Check 'Component.h' for more details
#define MARK_SERIALIZABLE(COMPONENT) \
    friend class Serializer; \
    \
	std::string GetName () const \
	{ \
		return #COMPONENT; \
	}

/**
 * Serialized field types:
 *
 * NO const or unsigned
 * Assignments are ALLOWED (even encouraged)
 *
 * bool <name> [= ...];
 * int <name> [= ...];
 * float <name> [= ...];
 * glm::vec2 <name> [= ...];
 * glm::vec3 <name> [= ...];
 * glm::vec4 <name> [= ...];
 * <EnumName> <name> [= ...];
 * Transform* <name> [= ...];
 * rendering::Texture* <name> [= ...];
 * rendering::Material* <name> [= ...];
 * 
 * 
 */

/**
 * Enum structure:
 *
 * SERIALIZE_ENUM
 * enum <name> {
 *   <value1>,
 *   <value2>,
 *   ...
 *   <valueN>
 * }
 *
 */

struct SerializedField;

namespace transform {
    class Transform;
}

class GUIManager;

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

        bool IsActive() const;
        void SetActive(bool active);

        transform::Transform* transform;

        virtual std::string GetName() const = 0;

    protected:
        bool enabled = true;
        bool hasAwakeActivated = false;
        bool hasStartActivated = false;

        friend class transform::Transform;
        friend class GUIManager;
    };
}   // namespace component
