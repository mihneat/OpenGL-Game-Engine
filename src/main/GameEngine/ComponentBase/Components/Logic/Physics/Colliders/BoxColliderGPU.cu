#include "BoxColliderGPU.cuh"

#include <iostream>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

__device__ ClosestPointsResult k_ClosestPointsBetweenLines(glm::vec3 p1, glm::vec3 v1, glm::vec3 p2, glm::vec3 v2)
{
    // TODO: What should be done if v1 and v2 are orthogonal? kc would be 0, and the hit point would be NaN..
    // This might not happen in the edge-edge collision check (it did when it had a mistake), but could happen to others
            
    glm::vec3 v1_n = glm::normalize(v1);
    glm::vec3 v2_n = glm::normalize(v2);
            
    // General case
    float kc = glm::dot(v1, v2);
    float kt1 = glm::dot(v1, v1);
    float ks2 = glm::dot(v2, v2);
    float k1 = glm::dot(v1, p2 - p1);
    float k2 = glm::dot(v2, p2 - p1);

    float t;
    if (glm::dot(v1_n, v2_n) > 0.999999f)
        // Directions are parallel, choose a random t
            t = 0.5f;
    else
        // General case, a single closest point
            t = (k1 * ks2 - k2 * kc) / (ks2 * kt1 - kc * kc);
            
    float s = (t * kt1 - k1) / kc;

    bool pOnSegment = t >= 0.0f && t <= 1.0f;
    bool qOnSegment = s >= 0.0f && s <= 1.0f;

    return { pOnSegment, qOnSegment, p1 + t * v1, p2 + s * v2 };
}

__device__ bool BoxCollider_Dev::k_CheckSeparatingPlaneForBoxes(const glm::vec3& rPos, const glm::vec3& plane, const BoxCollider_Dev* boxA, const BoxCollider_Dev* boxB, CollisionHit_Dev* hit, float& minimumOverlap, int& currentOverlapIndex, int& minimumOverlapIndex) const
{
    ++currentOverlapIndex;
    
    // Corner case
    if (glm::length2(plane) < 0.0001f)
        return false;
    
    glm::vec3 boxAX = boxA->transform.right;
    glm::vec3 boxAY = boxA->transform.up;
    glm::vec3 boxAZ = boxA->transform.forward;

    glm::vec3 boxBX = boxB->transform.right;
    glm::vec3 boxBY = boxB->transform.up;
    glm::vec3 boxBZ = boxB->transform.forward;

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
        minimumOverlapIndex = currentOverlapIndex;

        if (glm::dot(hitNormal, boxA->transform.worldPosition - boxB->transform.worldPosition) < 0.0f)
            hitNormal = -hitNormal;
        
        hit->normal = hitNormal;

        // This is a very raw approximation of the collision point, but finding the correct collision point is VERY tough
        glm::vec3 vectorToMiddle = hitNormal * (halfSizeProjectionsA - overlap / 2.0f);
        hit->point = boxA->transform.worldPosition + vectorToMiddle;
    }

    return false;
}

__device__ Edge BoxCollider_Dev::k_GetClosestEdgeInDirectionOfAxis(char pDirChar, glm::vec3 axis, glm::vec3 otherBoxCenter) const
{
    glm::vec3 pVec, oVec1, oVec2;
    switch (pDirChar)
    {
    case 'x':
        pVec = transform.right * halfSize.x;
        oVec1 = transform.up * halfSize.y;
        oVec2 = transform.forward * halfSize.z;
        break;
    case 'y':
        pVec = transform.up * halfSize.y;
        oVec1 = transform.right * halfSize.x;
        oVec2 = transform.forward * halfSize.z;
        break;
    case 'z':
        pVec = transform.forward * halfSize.z;
        oVec1 = transform.right * halfSize.x;
        oVec2 = transform.up * halfSize.y;
        break;
    default:
        printf("You must use either x, y or z\n");
        return { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) };
    }
    
    glm::vec3 center = transform.worldPosition;

    // Update the axis if needed
    glm::vec3 rPos = otherBoxCenter - center;
    if (glm::dot(axis, rPos) < 0.0f)
        axis = -axis;

    // Get the centers of each edge
    glm::vec3 ps[] = { center + oVec1 + oVec2, center + oVec1 - oVec2, center - oVec1 + oVec2, center - oVec1 - oVec2 };

    glm::vec3 maxPoint;
    float maxValue = -9999.0f;
    for (int i = 0; i < 4; ++i)
    {
        float pValue = glm::dot(axis, glm::normalize(ps[i] - center));
        if (maxValue < pValue)
        {
            maxValue = pValue;
            maxPoint = ps[i];
        }
    }

    return { maxPoint - pVec, 2.0f * pVec };
}

__device__ void k_CheckEdgeToEdgeCollision(const BoxCollider_Dev* boxA, char pDirCharA, const BoxCollider_Dev* boxB, char pDirCharB, glm::vec3 axis, CollisionHit_Dev* hit)
{
    // Find the closest edges
    Edge edgeA = boxA->k_GetClosestEdgeInDirectionOfAxis(pDirCharA, axis, boxB->transform.worldPosition);
    Edge edgeB = boxB->k_GetClosestEdgeInDirectionOfAxis(pDirCharB, axis, boxA->transform.worldPosition);

    // Find the closest contact points
    ClosestPointsResult pq = k_ClosestPointsBetweenLines(edgeA.start, edgeA.dirToEnd, edgeB.start, edgeB.dirToEnd);

    // Check if the segments intersect
    if (pq.pOnSegment == false || pq.qOnSegment == false)
        return;

    // Set the point halfway through
    hit->point = (pq.p1 + pq.p2) / 2.0f;
}

__device__ bool BoxCollider_Dev::k_CheckPointIsInside(glm::vec3 point) const
{
    // Move the point to the box's local coordinate system
    glm::vec3 boxToPoint = point - this->transform.worldPosition;
    glm::vec3 localPoint = glm::vec3(
        glm::dot(boxToPoint, this->transform.right),
        glm::dot(boxToPoint, this->transform.up),
        glm::dot(boxToPoint, this->transform.forward)
        );

    if (localPoint.x < -halfSize.x || halfSize.x < localPoint.x)
        return false;

    if (localPoint.y < -halfSize.y || halfSize.y < localPoint.y)
        return false;

    if (localPoint.z < -halfSize.z || halfSize.z < localPoint.z)
        return false;

    return true;
}

__device__ void BoxCollider_Dev::k_CheckFaceFaceCollision(const BoxCollider_Dev* referenceBox, const BoxCollider_Dev* incidentBox, glm::vec3 refPlaneCenter, CollisionHit_Dev* hit, bool useRetryFallback) const
{
    // The reference face is the one pointing in the same direction as the axis
    glm::vec3 referencePlaneDir = glm::normalize(refPlaneCenter - referenceBox->transform.worldPosition);

    // The incident face is the one whose direction has the smallest dot product value with the axis
    glm::vec3 incidentPlaneDirs[] = { incidentBox->transform.right, -incidentBox->transform.right, incidentBox->transform.up, -incidentBox->transform.up, incidentBox->transform.forward, -incidentBox->transform.forward };
    char incidentPlaneDirTypes[] = { 'x', 'y', 'z' };
    glm::vec3 incidentPlaneDir;
    char incidentPlaneDirType;
    float smallestValue = 9999.0f;

    for (int i = 0; i < 6; ++i)
    {
        float currValue = glm::dot(referencePlaneDir, incidentPlaneDirs[i]);
        if (currValue < smallestValue)
        {
            smallestValue = currValue;
            incidentPlaneDir = incidentPlaneDirs[i];
            incidentPlaneDirType = incidentPlaneDirTypes[i / 2];
        }
    }

    // Find the incident plane's vertices
    glm::vec3 incidentPlaneCenter;
    glm::vec3 oVec1, oVec2;
    switch (incidentPlaneDirType)
    {
    case 'x':
        incidentPlaneCenter = incidentBox->transform.worldPosition + incidentPlaneDir * incidentBox->halfSize.x;
        oVec1 = incidentBox->transform.up * incidentBox->halfSize.y;
        oVec2 = incidentBox->transform.forward * incidentBox->halfSize.z;
        break;

    case 'y':
        incidentPlaneCenter = incidentBox->transform.worldPosition + incidentPlaneDir * incidentBox->halfSize.y;
        oVec1 = incidentBox->transform.right * incidentBox->halfSize.x;
        oVec2 = incidentBox->transform.forward * incidentBox->halfSize.z;
        break;

    case 'z':
        incidentPlaneCenter = incidentBox->transform.worldPosition + incidentPlaneDir * incidentBox->halfSize.z;
        oVec1 = incidentBox->transform.right * incidentBox->halfSize.x;
        oVec2 = incidentBox->transform.up * incidentBox->halfSize.y;
        break;

    default:
        printf("Invalid direction type\n");
        hit->point = refPlaneCenter;
        return;
    }
    
    glm::vec3 incidentVertices[4];
    incidentVertices[0] = incidentPlaneCenter + oVec1 + oVec2;
    incidentVertices[1] = incidentPlaneCenter + oVec1 - oVec2;
    incidentVertices[2] = incidentPlaneCenter - oVec1 + oVec2;
    incidentVertices[3] = incidentPlaneCenter - oVec1 - oVec2;

    // Warning: this is ALSO a crude approximation of the contact point, but (should be) MUCH more accurate than the last
    // Compute the average of the contact points
    int len = 0;
    glm::vec3 contactPoints[4];
    glm::vec3 averagePoint(0.0f);

    for(const auto& incidentVertex : incidentVertices)
    {
        // Another approximation; check the commented code below
        if (referenceBox->k_CheckPointIsInside(incidentVertex))
        {
            contactPoints[len++] = incidentVertex;
            averagePoint += incidentVertex;
        }
    }

    if (len == 0)
    {
        if (useRetryFallback)
            k_CheckFaceFaceCollision(incidentBox, referenceBox, incidentPlaneCenter, hit, false);
        else
            hit->point = refPlaneCenter;
    } else
    {
        hit->point = averagePoint / static_cast<float>(len);
    }
}

__device__ bool BoxCollider_Dev::k_CollidesWithBox(const BoxCollider_Dev* other, CollisionHit_Dev* hit) const
{
    glm::vec3 rPos = other->transform.worldPosition - this->transform.worldPosition;

    glm::vec3 boxAX = this->transform.right;
    glm::vec3 boxAY = this->transform.up;
    glm::vec3 boxAZ = this->transform.forward;

    glm::vec3 boxBX = other->transform.right;
    glm::vec3 boxBY = other->transform.up;
    glm::vec3 boxBZ = other->transform.forward;

    int currentOverlapIndex = -1;
    int minimumOverlapIndex = -1;
    float minimumOverlap = 1000000.0f;

    hit->normal = glm::vec3(0.f, 1.f, 0.f);
    hit->point = this->transform.worldPosition;
    
    hit->hasHit = !(
        k_CheckSeparatingPlaneForBoxes(rPos, boxAX, this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, boxAY, this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, boxAZ, this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||

        k_CheckSeparatingPlaneForBoxes(rPos, boxBX, this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, boxBY, this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, boxBZ, this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        
        k_CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAX, boxBX), this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAX, boxBY), this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAX, boxBZ), this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAY, boxBX), this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAY, boxBY), this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAY, boxBZ), this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAZ, boxBX), this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAZ, boxBY), this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        k_CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAZ, boxBZ), this, other, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex)
    );

    if (!hit->hasHit)
        return false;

    switch (minimumOverlapIndex)
    {
    case 0:
        k_CheckFaceFaceCollision(this, other, this->transform.worldPosition - hit->normal * this->halfSize.x, hit);
        break;
    case 1:
        k_CheckFaceFaceCollision(this, other, this->transform.worldPosition - hit->normal * this->halfSize.y, hit);
        break;
    case 2:
        k_CheckFaceFaceCollision(this, other, this->transform.worldPosition - hit->normal * this->halfSize.z, hit);
        break;
    case 3:
        k_CheckFaceFaceCollision(other, this, other->transform.worldPosition + hit->normal * other->halfSize.x, hit);
        break;
    case 4:
        k_CheckFaceFaceCollision(other, this, other->transform.worldPosition + hit->normal * other->halfSize.y, hit);
        break;
    case 5:
        k_CheckFaceFaceCollision(other, this, other->transform.worldPosition + hit->normal * other->halfSize.z, hit);
        break;
    case 6:
        k_CheckEdgeToEdgeCollision(this, 'x', other, 'x', hit->normal, hit);
        break;
    case 7:
        k_CheckEdgeToEdgeCollision(this, 'x', other, 'y', hit->normal, hit);
        break;
    case 8:
        k_CheckEdgeToEdgeCollision(this, 'x', other, 'z', hit->normal, hit);
        break;
    case 9:
        k_CheckEdgeToEdgeCollision(this, 'y', other, 'x', hit->normal, hit);
        break;
    case 10:
        k_CheckEdgeToEdgeCollision(this, 'y', other, 'y', hit->normal, hit);
        break;
    case 11:
        k_CheckEdgeToEdgeCollision(this, 'y', other, 'z', hit->normal, hit);
        break;
    case 12:
        k_CheckEdgeToEdgeCollision(this, 'z', other, 'x', hit->normal, hit);
        break;
    case 13:
        k_CheckEdgeToEdgeCollision(this, 'z', other, 'y', hit->normal, hit);
        break;
    case 14:
        k_CheckEdgeToEdgeCollision(this, 'z', other, 'z', hit->normal, hit);
        break;
    default:
        break;
    }

    return true;
}

__device__ bool BoxCollider_Dev::k_CollidesWithSphere(const SphereCollider_Dev* other, CollisionHit_Dev* hit) const
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
