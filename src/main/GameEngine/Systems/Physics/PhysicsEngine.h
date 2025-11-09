#pragma once
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Collider.h"

namespace physics
{
    class PhysicsEngine
    {
    public:
        void SimulatePhysics(transform::Transform* transform, float deltaTime);

    protected:
        void ResolveCollision(component::Collider* colliderA, component::Collider* colliderB, component::CollisionHit& hit);
        void ResolveCollisionWithFriction(component::Collider* colliderA, component::Collider* colliderB, component::CollisionHit& hit);
        void ResolveCollisionWithRotation(component::Collider* colliderA, component::Collider* colliderB, component::CollisionHit& hit);
        void ResolveCollisionWithRotationAndFriction(component::Collider* colliderA, component::Collider* colliderB, component::CollisionHit& hit);
    };
}
