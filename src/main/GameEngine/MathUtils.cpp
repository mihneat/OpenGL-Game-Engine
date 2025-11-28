#include "MathUtils.h"

float utils::Plane::GetSignedDistanceToPlane(const glm::vec3& point) const
{
    return glm::dot(normal, point) - distance;
}

utils::AABB::AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint)
{
    center = (minPoint + maxPoint) / 2.0f;
    extents = maxPoint - center + 0.001f;
}

bool utils::AABB::IsOnOrInFrontOfPlane(const Plane& plane) const
{
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = extents.x * std::abs(plane.normal.x) +
            extents.y * std::abs(plane.normal.y) + extents.z * std::abs(plane.normal.z);

    return -r <= plane.GetSignedDistanceToPlane(center);
}
