#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "core/window/window_object.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"

namespace component
{
    class Camera;

    class OrthoCameraFollow : public Component
    {
    public:
        OrthoCameraFollow(transform::Transform* transform, transform::Transform* followTarget) : Component(transform),
            followTarget(followTarget), cam(NULL), isFixed(true), minDimensions(glm::vec2(150, 150)),
            maxDimensions(glm::vec2(450, 450)), fixedDimensions(glm::vec2(550, 500)), zoom(1.1f), zoomSpeed(1.2f), window(NULL) { }

        ~OrthoCameraFollow() { }

        void Start();
        void Update(const float deltaTime);

        void InputUpdate(const float deltaTime, const int mods);
        void WindowResize(int width, int height);

    protected:
        component::Camera* cam;
        transform::Transform* followTarget;

        bool isFixed;
        float zoom;
        glm::vec2 minDimensions, maxDimensions, fixedDimensions;
        float zoomSpeed;

        WindowObject* window;
    };
}
