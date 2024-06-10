#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "core/window/window_object.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"

namespace component
{
    class Camera;

    SERIALIZE_CLASS
    class OrthoCameraFollow : public Component
    {
        MARK_SERIALIZABLE
        
    public:
        OrthoCameraFollow(transform::Transform* transform) : Component(transform) { }

        ~OrthoCameraFollow() { }

        void Start();
        void Update(const float deltaTime);

        void InputUpdate(const float deltaTime, const int mods);
        void WindowResize(int width, int height);

    protected:
        Camera* cam = nullptr;
        SERIALIZE_FIELD transform::Transform* followTarget = nullptr;

        SERIALIZE_FIELD bool isFixed = true;
        SERIALIZE_FIELD float zoom = 1.1f;
        SERIALIZE_FIELD glm::vec2 minDimensions = glm::vec2(150, 150);
        SERIALIZE_FIELD glm::vec2 maxDimensions = glm::vec2(450, 450);
        SERIALIZE_FIELD glm::vec2 fixedDimensions = glm::vec2(550, 500);
        SERIALIZE_FIELD float zoomSpeed = 1.2f;
    };
}
