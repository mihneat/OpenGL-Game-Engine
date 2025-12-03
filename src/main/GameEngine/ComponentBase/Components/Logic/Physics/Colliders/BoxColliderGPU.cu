#include "BoxColliderGPU.cuh"

__device__ bool BoxCollider_Dev::k_CollidesWithBox(const BoxCollider_Dev* other, CollisionHit_Dev* hit) const
{
    return false;
}

__device__ bool BoxCollider_Dev::k_CollidesWithSphere(const SphereCollider_Dev* other, CollisionHit_Dev* hit) const
{
    return false;
}
