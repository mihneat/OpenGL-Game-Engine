#pragma once
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    SERIALIZE_CLASS

    class PropellerRotation : public Component
    {
        MARK_SERIALIZABLE(PropellerRotation)

        PropellerRotation(transform::Transform* transform) : Component(transform)
        {
        }

    public:
        void Update(float deltaTime) override;

    private:
        SERIALIZE_FIELD float speed = 2;
        SERIALIZE_FIELD glm::vec3 rotationAxis = glm::vec3(0, 1, 0);
    };
}
