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
    class DistanceDisplay : public Component, public IResetable
    {
        MARK_SERIALIZABLE(DistanceDisplay)
        
    public:
        DistanceDisplay(transform::Transform* transform) : Component(transform), textRenderer(NULL),
            distance(0.0f), prevPos(glm::vec3()) { }
        ~DistanceDisplay() { }

        void Start();
        void Update(const float deltaTime);

        void Reset();

    protected:
        SERIALIZE_FIELD transform::Transform* player = nullptr;
        component::TextRenderer* textRenderer;
        
        float distance;
        glm::vec3 prevPos;

    };
}
