#include "BoxCollider.h"

#include <iostream>
#include <GLFW/glfw3native.h>

#include "SphereCollider.h"

using namespace component;

// Thanks to: https://stackoverflow.com/a/52010428
// And to: https://www.youtube.com/watch?v=EB6NY5sGd08

bool BoxCollider::CheckSeparatingPlaneForBoxes(const glm::vec3& rPos, const glm::vec3& plane, const BoxCollider* boxA, const BoxCollider* boxB, CollisionHit& hit, float& minimumOverlap) const
{
    // Corner case
    if (glm::length2(plane) < 0.0001f)
        return false;
    
    glm::vec3 boxAX = boxA->transform->right;
    glm::vec3 boxAY = boxA->transform->up;
    glm::vec3 boxAZ = boxA->transform->forward;

    glm::vec3 boxBX = boxB->transform->right;
    glm::vec3 boxBY = boxB->transform->up;
    glm::vec3 boxBZ = boxB->transform->forward;

    // If a plane can separate the boxes, then the boxes are not colliding
    float boxCenterProjection = fabs(glm::dot(rPos, plane));
    float halfSizeProjectionsA =
        fabs(glm::dot(boxAX * boxA->halfSize.x, plane)) +
        fabs(glm::dot(boxAY * boxA->halfSize.y, plane)) +
        fabs(glm::dot(boxAZ * boxA->halfSize.z, plane));
    float halfSizeProjectionsB =
        fabs(glm::dot(boxBX * boxB->halfSize.x, plane)) + 
        fabs(glm::dot(boxBY * boxB->halfSize.y, plane)) +
        fabs(glm::dot(boxBZ * boxB->halfSize.z, plane));
    float halfSizeProjections = halfSizeProjectionsA + halfSizeProjectionsB;
    
    bool hasSeparatingPlane = boxCenterProjection > halfSizeProjections;
    if (hasSeparatingPlane)
        return true;
    
    // Check if it's the minimum overlap
    float overlap = halfSizeProjections - boxCenterProjection;
    glm::vec3 hitNormal = glm::normalize(plane);
    if (overlap < minimumOverlap)
    {
        minimumOverlap = overlap;
        
        hit.normal = hitNormal;

        // This is a very raw approximation of the collision point, but finding the correct collision point is VERY tough
        glm::vec3 vectorToMiddle = hitNormal * (halfSizeProjectionsA - overlap / 2.0f);
        hit.point = boxA->transform->GetWorldPosition() + vectorToMiddle;
        
        // glm::vec3 vectorToMiddleA = boxA->transform->GetWorldPosition() + hitNormal * halfSizeProjectionsA;
        // glm::vec3 vectorToMiddleB = boxB->transform->GetWorldPosition() - hitNormal * halfSizeProjectionsB;
        // hit.point = (vectorToMiddleA + vectorToMiddleA) / 2.0f;
    }

    return false;
}

bool BoxCollider::CheckBoxCollision(const BoxCollider* boxA, const BoxCollider* boxB, CollisionHit& hit) const
{
    glm::vec3 rPos = boxB->transform->GetWorldPosition() - boxA->transform->GetWorldPosition();

    glm::vec3 boxAX = boxA->transform->right;
    glm::vec3 boxAY = boxA->transform->up;
    glm::vec3 boxAZ = boxA->transform->forward;

    glm::vec3 boxBX = boxB->transform->right;
    glm::vec3 boxBY = boxB->transform->up;
    glm::vec3 boxBZ = boxB->transform->forward;

    float minimumOverlap = 1000000.0f;

    hit.normal = glm::vec3_up;
    hit.point = boxA->transform->GetWorldPosition();
    
    hit.hasHit = !(
        CheckSeparatingPlaneForBoxes(rPos, boxAX, boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, boxAY, boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, boxAZ, boxA, boxB, hit, minimumOverlap) ||

        CheckSeparatingPlaneForBoxes(rPos, boxBX, boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, boxBY, boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, boxBZ, boxA, boxB, hit, minimumOverlap) ||
        
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAX, boxBX), boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAX, boxBY), boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAX, boxBZ), boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAY, boxBX), boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAY, boxBY), boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAY, boxBZ), boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAZ, boxBX), boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAZ, boxBY), boxA, boxB, hit, minimumOverlap) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAZ, boxBZ), boxA, boxB, hit, minimumOverlap)
    );

    return hit.hasHit;
}

// Thanks to: https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection#sphere_vs._aabb
// And to: https://gamedev.stackexchange.com/questions/163873/separating-axis-theorem-obb-vs-sphere

bool BoxCollider::CheckSphereCollision(const BoxCollider* box, const SphereCollider* sphere, CollisionHit& hit) const
{
    // Transform the sphere's center into the box's local coords
    glm::vec3 obbToSphere = sphere->transform->GetWorldPosition() - box->transform->GetWorldPosition();
    glm::vec3 sphereLocalCenter = glm::vec3(
        glm::dot(obbToSphere, box->transform->right),
        glm::dot(obbToSphere, box->transform->up),
        glm::dot(obbToSphere, box->transform->forward)
    );
    
    // Get the box's closest point to the sphere center by clamping
    float x = glm::clamp(sphereLocalCenter.x, -box->halfSize.x, box->halfSize.x);
    float y = glm::clamp(sphereLocalCenter.y, -box->halfSize.y, box->halfSize.y);
    float z = glm::clamp(sphereLocalCenter.z, -box->halfSize.z, box->halfSize.z);
    
    float distance = glm::sqrt(
      (x - sphereLocalCenter.x) * (x - sphereLocalCenter.x) +
      (y - sphereLocalCenter.y) * (y - sphereLocalCenter.y) +
      (z - sphereLocalCenter.z) * (z - sphereLocalCenter.z)
    );

    hit.hasHit = distance < sphere->radius;
    if (!hit.hasHit)
        return false;

    hit.point = box->transform->GetWorldPosition() + x * box->transform->right + y * box->transform->up + z * box->transform->forward;
    
    glm::vec3 hitNormal = sphere->transform->GetWorldPosition() - hit.point;
    if (glm::length(hitNormal) < 0.00001f)
        hit.normal = glm::vec3_up;
    else
        hit.normal = glm::normalize(hitNormal);

    return true;
}

bool BoxCollider::CollidesWith(Collider* other, CollisionHit& hit)
{
    if (dynamic_cast<SphereCollider*>(other) != nullptr)
        return CheckSphereCollision(this, dynamic_cast<SphereCollider*>(other), hit);

    if (dynamic_cast<BoxCollider*>(other) != nullptr)
        return CheckBoxCollision(this, dynamic_cast<BoxCollider*>(other), hit);
    
    return false;
}

// Check: https://en.wikipedia.org/wiki/List_of_moments_of_inertia
float BoxCollider::GetMomentOfInertia(float mass)
{
    // Very crude approximation (normally we should detect which side of the box was hit, but we'll be using cubes)
    float sizeApproximation = (halfSize.x + halfSize.y + halfSize.z) / 3.0f;
    return 1.0f / 6.0f * mass * sizeApproximation * sizeApproximation;
}
