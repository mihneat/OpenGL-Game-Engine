#pragma once
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    SERIALIZE_CLASS

    class FractalTreeRoot : public Component
    {
        MARK_SERIALIZABLE(FractalTreeRoot)

    public:
        
        FractalTreeRoot(transform::Transform* transform) : Component(transform)
        {
        }

        void Init(float newSelectionValue);
        
        void Start() override;

    private:
        SERIALIZE_FIELD float selectionValue = 1.1f;
    };
}
