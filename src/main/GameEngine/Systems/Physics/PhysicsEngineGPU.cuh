#pragma once
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Colliders/BoxColliderGPU.cuh"
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Colliders/SphereColliderGPU.cuh"

__global__ void k_ProcessCollisions(CollisionHit_Dev* hits, BoxCollider_Dev* boxColliders_d, SphereCollider_Dev* sphereColliders_d, char* transformNames_d, int boxCnt, int sphereCnt);

cudaError_t ProcessCollisionsOnGPU(CollisionHit_Dev* hits, BoxCollider_Dev* boxColliders_d, SphereCollider_Dev* sphereColliders_d,
    char* transformNames_d, int boxCnt, int sphereCnt, dim3 DIM_GRID, dim3 DIM_BLOCK);
