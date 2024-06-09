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
        CameraFollow(transform::Transform* transform, transform::Transform* followTarget,
            float distanceToTarget, float forwardFollowDistance) : Component(transform), followTarget(followTarget),
            distanceToTarget(distanceToTarget), mainCam(NULL), isRetroCam(false),
            forwardFollowDistance(forwardFollowDistance), forwardOffset(glm::vec3()), angleScale(0.7f), retroAngleScale(0.05f)
        { }

        ~CameraFollow() { }

        void Start();
        void LateUpdate(const float deltaTime);

        void KeyPress(const int key, const int mods);
        void KeyRelease(const int key, const int mods);

    protected:
        Camera* mainCam;
        SERIALIZE_FIELD transform::Transform* followTarget = nullptr;
        SERIALIZE_FIELD float distanceToTarget;

        SERIALIZE_FIELD float forwardFollowDistance;
        glm::vec3 forwardOffset;
        
        SERIALIZE_FIELD float angleScale;
        SERIALIZE_FIELD float retroAngleScale;

        SERIALIZE_FIELD bool isRetroCam;
    };
}
