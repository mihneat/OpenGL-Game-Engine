#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "lab_m1/Homework/3 - Steep/MathUtils.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/TextRenderer.h"

namespace component
{
    class TextRenderer;

    class SpeedSelectionDisplay : public Component
    {
    public:
        SpeedSelectionDisplay(transform::Transform* transform) : Component(transform), textRenderer(NULL),
            prevTextRenderer(NULL), currTextRenderer(NULL), nextTextRenderer(NULL) { }
        ~SpeedSelectionDisplay() { }

        void Start();
        void Update(const float deltaTime);

        void SetTextValues(component::TextRenderer* textRenderer, int gameSpeed, bool fade = false);
        void EmptyText(component::TextRenderer* textRenderer);

    protected:
        component::TextRenderer* textRenderer;
        component::TextRenderer* prevTextRenderer;
        component::TextRenderer* currTextRenderer;
        component::TextRenderer* nextTextRenderer;
    };
}
