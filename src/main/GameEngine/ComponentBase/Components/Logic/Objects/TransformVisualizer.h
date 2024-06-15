#pragma once
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    SERIALIZE_ENUM
    enum TransformDirection
    {
        TransformDirectionForward,
        TransformDirectionRight,
        TransformDirectionUp,
        TransformDirectionCenter
    };
    
    SERIALIZE_CLASS
    class TransformVisualizer : public Component
    {
        MARK_SERIALIZABLE(TransformVisualizer)

        TransformVisualizer(transform::Transform* transform) : Component(transform) { }
        
        void Start() override;
        void Update(float deltaTime) override;

    private:
        SERIALIZE_FIELD TransformDirection direction = TransformDirectionForward;
        SERIALIZE_FIELD float distance = 5.0f;

        transform::Transform* player = nullptr;
    };
}
