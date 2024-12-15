#pragma once
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"

namespace component
{
    SERIALIZE_CLASS
    class MinimapCamera : public Component
    {
        MARK_SERIALIZABLE(MinimapCamera)

        MinimapCamera(transform::Transform* transform) : Component(transform)
        {
        }

    public:
        void Start() override;

        void WindowResize(int width, int height) override;

        Camera* camera = nullptr;
    };
}
