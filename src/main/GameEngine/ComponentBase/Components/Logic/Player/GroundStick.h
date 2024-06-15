#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    SERIALIZE_CLASS
    class GroundStick : public Component
    {
        MARK_SERIALIZABLE(GroundStick)
        
    public:
        GroundStick(transform::Transform* transform) : Component(transform) { }
        ~GroundStick() { }

        void Start();
        void Update(const float deltaTime);

    protected:
        SERIALIZE_FIELD transform::Transform* player = nullptr;
        SERIALIZE_FIELD glm::vec3 offset = glm::vec3();
    };
}
