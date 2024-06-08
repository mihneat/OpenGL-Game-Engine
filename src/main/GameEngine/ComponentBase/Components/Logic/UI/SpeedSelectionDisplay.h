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
    class SpeedSelectionDisplay : public Component
    {
        MARK_SERIALIZABLE
        
    public:
        SpeedSelectionDisplay(transform::Transform* transform) : Component(transform), textRenderer(NULL),
            prevTextRenderer(NULL), currTextRenderer(NULL), nextTextRenderer(NULL) { }
        ~SpeedSelectionDisplay() { }

        void Start();
        void Update(const float deltaTime);

        void SetTextValues(TextRenderer* textRenderer, int gameSpeed, bool fade = false);
        void EmptyText(TextRenderer* textRenderer);

    protected:
        GameManager* gameManager = nullptr;
        TextRenderer* textRenderer;
        TextRenderer* prevTextRenderer;
        TextRenderer* currTextRenderer;
        TextRenderer* nextTextRenderer;
    };
}
