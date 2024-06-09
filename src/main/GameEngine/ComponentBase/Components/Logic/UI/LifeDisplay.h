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
    class LifeDisplay : public Component
    {
        MARK_SERIALIZABLE
        
    public:
        LifeDisplay(transform::Transform* transform) : Component(transform) { }
        ~LifeDisplay() { }

        void Start();
        void Update(const float deltaTime);

    protected:
        SERIALIZE_FIELD transform::Transform* player = nullptr;
    };
}
