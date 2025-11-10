#include "SphereCollider.h"

#include "BoxCollider.h"

using namespace component;

bool SphereCollider::CollidesWith(Collider* other, CollisionHit& hit)
{
    if (dynamic_cast<SphereCollider*>(other) != nullptr)
    {
        SphereCollider* otherSphere = dynamic_cast<SphereCollider*>(other);
        
        glm::vec3 thisPosition = this->transform->GetWorldPosition();
        glm::vec3 otherPosition = otherSphere->transform->GetWorldPosition();
        
        float sphereDistance = glm::distance(thisPosition, otherPosition);
        float totalRadius = this->radius + otherSphere->radius;
        hit.hasHit = sphereDistance < totalRadius;

        // Quit early in case of no collision
        if (!hit.hasHit)
            return false;

        glm::vec3 hitNormal = (thisPosition - otherPosition) / sphereDistance;
        hit.normal = hitNormal;

        glm::vec3 vectorToMiddle = -hitNormal * (this->radius - (totalRadius - sphereDistance) / 2.0f);
        hit.point = thisPosition + vectorToMiddle;

        return hit.hasHit;
    }

    if (dynamic_cast<BoxCollider*>(other) != nullptr)
    {
        // Only implement the function in one place, but invert the normal
        other->CollidesWith(this, hit);
        hit.normal = -hit.normal;
        
        return hit.hasHit;
    }
    
    return false;
}

// Check: https://en.wikipedia.org/wiki/List_of_moments_of_inertia
float SphereCollider::GetMomentOfInertia(float mass)
{
    return 2.0f / 5.0f * mass * radius * radius;
}
