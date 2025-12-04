#include "SphereColliderGPU.cuh"

#include <iostream>
#include <glm/geometric.hpp>

#include "BoxColliderGPU.cuh"

__device__ bool SphereCollider_Dev::k_CollidesWithSphere(const SphereCollider_Dev* other, CollisionHit_Dev* hit) const
{
    glm::vec3 thisPosition = this->transform.worldPosition;
    glm::vec3 otherPosition = other->transform.worldPosition;
    
    // printf("World positions: [%f, %f, %f], [%f, %f, %f]\n", thisPosition.x, thisPosition.y, thisPosition.z, otherPosition.x, otherPosition.y, otherPosition.z);
    // printf("Radii: %f, %f\n", this->radius, other->radius);
        
    float sphereDistance = glm::distance(thisPosition, otherPosition);
    float totalRadius = this->radius + other->radius;
    hit->hasHit = sphereDistance < totalRadius;

    // printf("Sphere distance: %f\n", sphereDistance);
    // printf("Total radius: %f\n", totalRadius);
    // printf("Has hit: %d\n\n", hit->hasHit);
    

    // Quit early in case of no collision
    if (!hit->hasHit)
        return false;

    glm::vec3 hitNormal = (thisPosition - otherPosition) / sphereDistance;
    hit->normal = hitNormal;

    glm::vec3 vectorToMiddle = -hitNormal * (this->radius - (totalRadius - sphereDistance) / 2.0f);
    hit->point = thisPosition + vectorToMiddle;

    return hit->hasHit;
}

__device__ bool SphereCollider_Dev::k_CollidesWithBox(const BoxCollider_Dev* other, CollisionHit_Dev* hit) const
{
    other->k_CollidesWithSphere(this, hit);
    hit->normal = -hit->normal;
        
    return hit->hasHit;
}
