#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "lab_m1/Homework/3 - Steep/MathUtils.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/TextRenderer.h"

namespace component
{
    class TextRenderer;

    class DistanceDisplay : public Component, public IResetable
    {
    public:
        DistanceDisplay(transform::Transform* transform) : Component(transform), textRenderer(NULL),
            distance(0.0f), prevPos(glm::vec3()) { }
        ~DistanceDisplay() { }

        void Start();
        void Update(const float deltaTime);

        void Reset();

    protected:
        transform::Transform* player;
        component::TextRenderer* textRenderer;
        
        float distance;
        glm::vec3 prevPos;

    };
}
