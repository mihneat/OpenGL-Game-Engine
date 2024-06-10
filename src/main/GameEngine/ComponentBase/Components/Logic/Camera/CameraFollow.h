#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"

namespace component
{
    class Camera;

    SERIALIZE_CLASS
    class CameraFollow : public Component
    {
        MARK_SERIALIZABLE
        
    public:
        CameraFollow(transform::Transform* transform, transform::Transform* followTarget = nullptr,
            float distanceToTarget = 100.0f, float forwardFollowDistance = 65.0f) : Component(transform), followTarget(followTarget),
            distanceToTarget(distanceToTarget), forwardFollowDistance(forwardFollowDistance)
        { }

        ~CameraFollow() { }

        void Start();
        void LateUpdate(const float deltaTime);

        void KeyPress(const int key, const int mods);
        void KeyRelease(const int key, const int mods);

    protected:
        Camera* mainCam = nullptr;
        SERIALIZE_FIELD transform::Transform* followTarget = nullptr;
        SERIALIZE_FIELD float distanceToTarget = 100.0f;

        SERIALIZE_FIELD float forwardFollowDistance = 65.0f;
        glm::vec3 forwardOffset = glm::vec3();
        
        SERIALIZE_FIELD float angleScale = 0.7f;
        SERIALIZE_FIELD float retroAngleScale = 0.05f;

        SERIALIZE_FIELD bool isRetroCam = false;
    };
}
