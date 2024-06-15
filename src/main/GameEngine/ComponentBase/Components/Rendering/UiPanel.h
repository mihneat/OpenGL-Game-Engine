#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/MeshRenderer.h"

namespace component
{
    class GameManager;
}

namespace component
{
    class MeshRenderer;

    SERIALIZE_CLASS
    class UiPanel : public Component
    {
        MARK_SERIALIZABLE(UiPanel)
        
    public:
        UiPanel(transform::Transform* transform) : Component(transform),
            anchorTop(false), anchorRight(false), offsetTop(0.0f), offsetRight(0.0f) { }
        ~UiPanel() { }
        
        void Update(const float deltaTime) override;

        void AnchorTop(const float offset);
        void AnchorRight(const float offset);

        void WindowResize(int width, int height);

    private:
        void UpdatePosition();

    protected:
        SERIALIZE_FIELD bool anchorTop;
        SERIALIZE_FIELD bool anchorRight;
        SERIALIZE_FIELD float offsetTop;
        SERIALIZE_FIELD float offsetRight;
    };
}
