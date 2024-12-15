#pragma once
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/MeshRenderer.h"

namespace component
{
    SERIALIZE_CLASS

    class FractalTreeSegment : public Component
    {
        MARK_SERIALIZABLE(FractalTreeSegment)

    public:
        FractalTreeSegment(transform::Transform* transform) : Component(transform)
        {
        }

        void Start() override;

        void Init(int newRemainingLevels, float newSelectionValue);
        
        MeshRenderer* meshRenderer = nullptr;

    private:
        SERIALIZE_FIELD int remainingLevels = 0;
        SERIALIZE_FIELD float selectionValue = 1.1f;

    };
}
