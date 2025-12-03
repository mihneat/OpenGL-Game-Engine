#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Collider.h"
#include "SphereColliderGPU.cuh"

namespace component
{
    SERIALIZE_CLASS
    class SphereCollider : public Collider
    {
        MARK_SERIALIZABLE(SphereCollider)
        
    public:
        SphereCollider(transform::Transform* transform, float radius = 0.0f) : Collider(transform), radius(radius) { }
        virtual ~SphereCollider() { }

        bool CollidesWith(Collider* other, CollisionHit& hit) override;
        float GetMomentOfInertia(float mass) override;

        void CloneToDevice(SphereCollider_Dev& sphereCollider_d);
        
        SERIALIZE_FIELD float radius = 0.0f;
        
    protected:
    };
}
