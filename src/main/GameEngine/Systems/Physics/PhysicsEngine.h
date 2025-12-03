#pragma once
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Collider.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Colliders/BoxCollider.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Colliders/BoxColliderGPU.cuh"

namespace physics
{
    class PhysicsEngine
    {
    public:
        PhysicsEngine(bool useGPU) : useGPUAcceleration(useGPU) { }
        ~PhysicsEngine();
        
        void SimulatePhysics(transform::Transform* transform, float deltaTime);

    protected:
        void ResolveCollision(component::Collider* colliderA, component::Collider* colliderB, component::CollisionHit& hit);
        void ResolveCollisionWithFriction(component::Collider* colliderA, component::Collider* colliderB, component::CollisionHit& hit);
        void ResolveCollisionWithRotation(component::Collider* colliderA, component::Collider* colliderB, component::CollisionHit& hit);
        void ResolveCollisionWithRotationAndFriction(component::Collider* colliderA, component::Collider* colliderB, component::CollisionHit& hit);
        
        void SimulatePhysicsCPU(transform::Transform* transform, float deltaTime);
        void SimulatePhysicsGPU(transform::Transform* transform, float deltaTime);

    private:
        inline void CopyDataHostToDevice(const std::vector<component::BoxCollider*>& boxColliders, const std::vector<component::SphereCollider*>& sphereColliders);
        inline void CopyDataDeviceToHost(std::vector<component::BoxCollider*>& boxColliders, std::vector<component::SphereCollider*>& sphereColliders);
        
        inline void FreeData();
        
        bool useGPUAcceleration = false;

        int prevBoxCnt = -1;
        int prevSphereCnt = -1;

        CollisionHit_Dev* hit_d = nullptr;
        BoxCollider_Dev* boxColliders_d = nullptr;
        SphereCollider_Dev* sphereColliders_d = nullptr;
        BoxCollider_Dev* boxColliders_h = nullptr;
        SphereCollider_Dev* sphereColliders_h = nullptr;
    };
}
