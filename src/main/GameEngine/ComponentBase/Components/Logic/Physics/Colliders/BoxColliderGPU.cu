#include "BoxColliderGPU.cuh"

#include <glm/geometric.hpp>

__host__ __device__ bool BoxCollider_Dev::k_CollidesWithBox(const BoxCollider_Dev* other, CollisionHit_Dev* hit) const
{
    return false;
}

__host__ __device__ bool BoxCollider_Dev::k_CollidesWithSphere(const SphereCollider_Dev* other, CollisionHit_Dev* hit) const
{
    // Transform the sphere's center into the box's local coords
    glm::vec3 obbToSphere = other->transform.worldPosition - this->transform.worldPosition;
    glm::vec3 sphereLocalCenter = glm::vec3(
        glm::dot(obbToSphere, this->transform.right),
        glm::dot(obbToSphere, this->transform.up),
        glm::dot(obbToSphere, this->transform.forward)
    );
    
    // Get the box's closest point to the sphere center by clamping
    float x = glm::clamp(sphereLocalCenter.x, -this->halfSize.x, this->halfSize.x);
    float y = glm::clamp(sphereLocalCenter.y, -this->halfSize.y, this->halfSize.y);
    float z = glm::clamp(sphereLocalCenter.z, -this->halfSize.z, this->halfSize.z);
    
    float distance = glm::sqrt(
      (x - sphereLocalCenter.x) * (x - sphereLocalCenter.x) +
      (y - sphereLocalCenter.y) * (y - sphereLocalCenter.y) +
      (z - sphereLocalCenter.z) * (z - sphereLocalCenter.z)
    );

    hit->hasHit = distance < other->radius;
    if (!hit->hasHit)
        return false;

    hit->point = this->transform.worldPosition + x * this->transform.right + y * this->transform.up + z * this->transform.forward;
    
    glm::vec3 hitNormal = other->transform.worldPosition - hit->point;
    if (glm::length(hitNormal) < 0.00001f)
        hit->normal = -glm::vec3(0.f, 1.f, 0.f);
    else
        hit->normal = -glm::normalize(hitNormal);

    return true;
}
