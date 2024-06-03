#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/TextRenderer.h"

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
