#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/Light.h"

namespace component
{
    SERIALIZE_CLASS
    class Sun : public Component
    {
        MARK_SERIALIZABLE
        
    public:
        Sun(transform::Transform* transform) : Component(transform), speed(0.1f), nightSpeed(0.25f), dist(10000.0f), lightSource(NULL), sunPosition(glm::vec3()) { }
        ~Sun() { }

        void Start();
        void Update(const float deltaTime);

    private:
        glm::vec3 sunPosition;
        Light* lightSource;

        SERIALIZE_FIELD float speed;
        SERIALIZE_FIELD float nightSpeed;
        float dist;
    };
}
