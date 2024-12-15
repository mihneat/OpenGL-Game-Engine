#pragma once
#include "main/GameEngine/ComponentBase/Component.h"

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

        void Init(int maxLevels);

    private:
        SERIALIZE_FIELD int remainingLevels = 0;
    };
}
