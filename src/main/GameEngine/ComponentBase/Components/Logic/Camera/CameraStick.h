#pragma once
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    SERIALIZE_CLASS

    class CameraStick : public Component
    {
        MARK_SERIALIZABLE(CameraStick)

        CameraStick(transform::Transform* transform) : Component(transform)
        {
        }

    public:
        void Start() override;
        void Update(const float deltaTime) override;

    private:
        glm::vec3 relativeOffset = glm::vec3();
        transform::Transform* helicopter = nullptr;
    };
}
