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

        void Start() override;      

        void Init();

    private:
        bool isInitialized = false;
    };
}
