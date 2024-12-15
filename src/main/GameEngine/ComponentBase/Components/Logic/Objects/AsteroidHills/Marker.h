#pragma once
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    SERIALIZE_CLASS

    class Marker : public Component
    {
        MARK_SERIALIZABLE(Marker)

    public:
        Marker(transform::Transform* transform) : Component(transform)
        {
        }
        
        void Start() override;
        void Update(float deltaTime) override;

        SERIALIZE_FIELD float speed = 5.0f;
        SERIALIZE_FIELD float rotationSpeed = 10.0f;
        
        transform::Transform* pointer = nullptr;
        glm::vec3 pointerInitialPosition = glm::vec3();
    };
}
