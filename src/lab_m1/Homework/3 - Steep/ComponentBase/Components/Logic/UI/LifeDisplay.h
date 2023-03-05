#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "lab_m1/Homework/3 - Steep/MathUtils.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/TextRenderer.h"

namespace component
{
    class TextRenderer;

    class LifeDisplay : public Component
    {
    public:
        LifeDisplay(transform::Transform* transform) : Component(transform) { }
        ~LifeDisplay() { }

        void Start();
        void Update(const float deltaTime);

    protected:
        transform::Transform* player;
    };
}
