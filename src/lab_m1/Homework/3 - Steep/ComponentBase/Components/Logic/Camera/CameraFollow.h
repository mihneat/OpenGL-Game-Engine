#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Camera.h"

namespace component
{
    class Camera;

    class CameraFollow : public Component
    {
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
        component::Camera* mainCam;
        transform::Transform* followTarget;
        float distanceToTarget;

        float forwardFollowDistance;
        glm::vec3 forwardOffset;
        
        const float angleScale, retroAngleScale;

        bool isRetroCam;
    };
}
