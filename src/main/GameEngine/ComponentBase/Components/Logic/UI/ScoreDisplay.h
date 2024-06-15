#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/TextRenderer.h"

namespace component
{
    class TextRenderer;

    SERIALIZE_CLASS
    class ScoreDisplay : public Component
    {
        MARK_SERIALIZABLE(ScoreDisplay)
        
    public:
        ScoreDisplay(transform::Transform* transform) : Component(transform), textRenderer(NULL) { }
        ~ScoreDisplay() { }

        void Start();
        void Update(const float deltaTime);

    protected:
        component::TextRenderer* textRenderer;
    };
}
