#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Lights/Light.h"

namespace component
{
    class Sun : public Component
    {
    public:
        Sun(transform::Transform* transform) : Component(transform), speed(0.1f), nightSpeed(0.25f), dist(10000.0f), lightSource(NULL), sunPosition(glm::vec3()) { }
        ~Sun() { }

        void Start();
        void Update(const float deltaTime);

    private:
        glm::vec3 sunPosition;
        component::Light* lightSource;

        float speed, nightSpeed;
        float dist;
    };
}
