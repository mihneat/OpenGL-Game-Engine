#include "Collider.h"

bool component::Collider::CollidesWith(Collider* other, CollisionHit& hit)
{
    return false;
}

float component::Collider::GetMomentOfInertia(float mass)
{
    return 1.0f;
}
