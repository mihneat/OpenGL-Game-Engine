#pragma once
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/DeviceStructs.cuh"

struct BoxCollider_Dev;

struct SphereCollider_Dev
{
    Transform_Dev transform;
    Rigidbody_Dev rb;
    
    float radius;

    __device__ bool k_CollidesWithSphere(const SphereCollider_Dev* other, CollisionHit_Dev* hit) const;
    __device__ bool k_CollidesWithBox(const BoxCollider_Dev* other, CollisionHit_Dev* hit) const;
};
