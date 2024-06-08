#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    SERIALIZE_CLASS
    class GroundStick : public Component
    {
        MARK_SERIALIZABLE
        
    public:
        GroundStick(transform::Transform* transform) : Component(transform), player(NULL), offset(glm::vec3()) { }
        ~GroundStick() { }

        void Start();
        void Update(const float deltaTime);

    protected:
        SERIALIZE_FIELD transform::Transform* player;
        SERIALIZE_FIELD glm::vec3 offset;
    };
}
