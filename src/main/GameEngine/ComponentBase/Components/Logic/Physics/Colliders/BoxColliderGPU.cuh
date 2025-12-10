#pragma once
#include "SphereColliderGPU.cuh"
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/DeviceStructs.cuh"
#include "main/GameEngine/Utils/CUDAUtils.cuh"

struct ClosestPointsResult
{
    bool pOnSegment, qOnSegment;
    glm::vec3 p1, p2;
};

struct Edge
{
    glm::vec3 start;
    glm::vec3 dirToEnd;
};

struct BoxCollider_Dev
{
    Transform_Dev transform;
    Rigidbody_Dev rb;
    
    glm::vec3 halfSize;

    __device__ bool k_CollidesWithBox(const BoxCollider_Dev* other, CollisionHit_Dev* hit, char* transformNames_d) const;
    __device__ bool k_CollidesWithSphere(const SphereCollider_Dev* other, CollisionHit_Dev* hit) const;

    __device__ void k_CheckFaceFaceCollision(const BoxCollider_Dev* referenceBox, const BoxCollider_Dev* incidentBox, glm::vec3 refPlaneCenter, CollisionHit_Dev* hit, char* transformNames_d, bool useRetryFallback = true) const;

    __device__ Edge k_GetClosestEdgeInDirectionOfAxis(char pDirChar, glm::vec3 axis, glm::vec3 otherBoxCenter) const;
    

private:
    __device__ bool k_CheckPointIsInside(glm::vec3 point) const;
    __device__ bool k_CheckSeparatingPlaneForBoxes(const glm::vec3& rPos, const glm::vec3& plane, const BoxCollider_Dev* boxA, const BoxCollider_Dev* boxB, CollisionHit_Dev* hit, float& minimumOverlap, int& currentOverlapIndex, int& minimumOverlapIndex) const;

};
