#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    class GroundStick : public Component
    {
    public:
        GroundStick(transform::Transform* transform) : Component(transform), player(NULL), offset(glm::vec3()) { }

        ~GroundStick() { }

        void Start();
        void Update(const float deltaTime);

    protected:
        transform::Transform* player;
        glm::vec3 offset;
    };
}
