#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/MeshRenderer.h"

namespace managers
{
    class GameManager;
}

namespace component
{
    class MeshRenderer;

    class UiPanel : public Component
    {
    public:
        UiPanel(transform::Transform* transform) : Component(transform),
            anchorTop(false), anchorRight(false), offsetTop(0.0f), offsetRight(0.0f) { }
        ~UiPanel() { }

        void AnchorTop(const float offset);
        void AnchorRight(const float offset);

        void WindowResize(int width, int height);

    protected:
        bool anchorTop, anchorRight;
        float offsetTop, offsetRight;
    };
}
