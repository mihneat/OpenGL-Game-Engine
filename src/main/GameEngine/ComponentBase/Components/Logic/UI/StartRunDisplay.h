#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/TextRenderer.h"

namespace component
{
    class TextRenderer;

    class StartRunDisplay : public Component
    {
    public:
        StartRunDisplay(transform::Transform* transform) : Component(transform), textRenderer(NULL) { }
        ~StartRunDisplay() { }

        void Start();
        void Update(const float deltaTime);

    protected:
        component::TextRenderer* textRenderer;
    };
}
