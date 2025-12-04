#include "BoxCollider.h"

#include <iostream>

#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Rigidbody.h"

using namespace component;
using namespace utils;

// Thanks to: https://stackoverflow.com/a/52010428
// And to: https://www.youtube.com/watch?v=EB6NY5sGd08

bool BoxCollider::CheckSeparatingPlaneForBoxes(const glm::vec3& rPos, const glm::vec3& plane, const BoxCollider* boxA, const BoxCollider* boxB, CollisionHit& hit, float& minimumOverlap, int& currentOverlapIndex, int& minimumOverlapIndex) const
{
    ++currentOverlapIndex;
    
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
        minimumOverlapIndex = currentOverlapIndex;

        if (glm::dot(hitNormal, boxA->transform->GetWorldPosition() - boxB->transform->GetWorldPosition()) < 0.0f)
            hitNormal = -hitNormal;
        
        hit.normal = hitNormal;

        // This is a very raw approximation of the collision point, but finding the correct collision point is VERY tough
        glm::vec3 vectorToMiddle = hitNormal * (halfSizeProjectionsA - overlap / 2.0f);
        hit.point = boxA->transform->GetWorldPosition() + vectorToMiddle;
    }

    return false;
}

std::pair<glm::vec3, glm::vec3> BoxCollider::GetClosestEdgeInDirectionOfAxis(char pDirChar, glm::vec3 axis, glm::vec3 otherBoxCenter) const
{
    glm::vec3 pVec, oVec1, oVec2;
    switch (pDirChar)
    {
    case 'x':
        pVec = transform->right * halfSize.x;
        oVec1 = transform->up * halfSize.y;
        oVec2 = transform->forward * halfSize.z;
        break;
    case 'y':
        pVec = transform->up * halfSize.y;
        oVec1 = transform->right * halfSize.x;
        oVec2 = transform->forward * halfSize.z;
        break;
    case 'z':
        pVec = transform->forward * halfSize.z;
        oVec1 = transform->right * halfSize.x;
        oVec2 = transform->up * halfSize.y;
        break;
    default:
        std::cerr << "You must use either x, y or z";
        return {};
    }
    
    glm::vec3 center = transform->GetWorldPosition();

    // Update the axis if needed
    glm::vec3 rPos = otherBoxCenter - center;
    if (glm::dot(axis, rPos) < 0.0f)
        axis = -axis;

    // Get the centers of each edge
    std::vector<glm::vec3> ps = { center + oVec1 + oVec2, center + oVec1 - oVec2, center - oVec1 + oVec2, center - oVec1 - oVec2 };

    glm::vec3 maxPoint;
    float maxValue = -9999.0f;
    for (int i = 0; i < ps.size(); ++i)
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

void CheckEdgeToEdgeCollision(const BoxCollider* boxA, char pDirCharA, const BoxCollider* boxB, char pDirCharB, glm::vec3 axis, CollisionHit& hit)
{
    // Find the closest edges
    std::pair<glm::vec3, glm::vec3> edgeA = boxA->GetClosestEdgeInDirectionOfAxis(pDirCharA, axis, boxB->transform->GetWorldPosition());
    std::pair<glm::vec3, glm::vec3> edgeB = boxB->GetClosestEdgeInDirectionOfAxis(pDirCharB, axis, boxA->transform->GetWorldPosition());

    // Find the closest contact points
    std::tuple<bool, bool, glm::vec3, glm::vec3> pq = MathUtils::ClosestPointsBetweenLines(edgeA.first, edgeA.second, edgeB.first, edgeB.second);

    // Check if the segments intersect
    if (std::get<0>(pq) == false || std::get<1>(pq) == false)
        return;

    // Set the point halfway through
    hit.point = (std::get<2>(pq) + std::get<3>(pq)) / 2.0f;
    
    // if ((boxA->transform->GetName() == "Box" && boxB->transform->GetName() == "Ground") ||
    //     (boxB->transform->GetName() == "Box" && boxA->transform->GetName() == "Ground"))
    // {
    //     std::cout << "Edge A: " << edgeA.first << " -> " << edgeA.first + edgeA.second << "\n";
    //     std::cout << "Edge B: " << edgeB.first << " -> " << edgeB.first + edgeB.second << "\n";
    //     std::cout << "PQ: " << std::get<2>(pq) << ", " << std::get<3>(pq) << "\n";
    //     std::cout << "\n";
    // }
}

bool BoxCollider::CheckPointIsInside(glm::vec3 point) const
{
    // Move the point to the box's local coordinate system
    glm::vec3 boxToPoint = point - this->transform->GetWorldPosition();
    glm::vec3 localPoint = glm::vec3(
        glm::dot(boxToPoint, this->transform->right),
        glm::dot(boxToPoint, this->transform->up),
        glm::dot(boxToPoint, this->transform->forward)
        );

    if (localPoint.x < -halfSize.x || halfSize.x < localPoint.x)
        return false;

    if (localPoint.y < -halfSize.y || halfSize.y < localPoint.y)
        return false;

    if (localPoint.z < -halfSize.z || halfSize.z < localPoint.z)
        return false;

    return true;
}

void BoxCollider::CheckFaceFaceCollision(const BoxCollider* referenceBox, const BoxCollider* incidentBox, glm::vec3 refPlaneCenter, CollisionHit& hit, bool useRetryFallback) const
{
    // The reference face is the one pointing in the same direction as the axis
    glm::vec3 referencePlaneDir = glm::normalize(refPlaneCenter - referenceBox->transform->GetWorldPosition());

    // The incident face is the one whose direction has the smallest dot product value with the axis
    std::vector<glm::vec3> incidentPlaneDirs = { incidentBox->transform->right, -incidentBox->transform->right, incidentBox->transform->up, -incidentBox->transform->up, incidentBox->transform->forward, -incidentBox->transform->forward };
    std::vector<char> incidentPlaneDirTypes = { 'x', 'y', 'z' };
    glm::vec3 incidentPlaneDir;
    char incidentPlaneDirType;
    float smallestValue = 9999.0f;

    for (int i = 0; i < incidentPlaneDirs.size(); ++i)
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
    std::vector<glm::vec3> incidentVertices;
    switch (incidentPlaneDirType)
    {
    case 'x':
        incidentPlaneCenter = incidentBox->transform->GetWorldPosition() + incidentPlaneDir * incidentBox->halfSize.x;
        oVec1 = incidentBox->transform->up * incidentBox->halfSize.y;
        oVec2 = incidentBox->transform->forward * incidentBox->halfSize.z;
        break;

    case 'y':
        incidentPlaneCenter = incidentBox->transform->GetWorldPosition() + incidentPlaneDir * incidentBox->halfSize.y;
        oVec1 = incidentBox->transform->right * incidentBox->halfSize.x;
        oVec2 = incidentBox->transform->forward * incidentBox->halfSize.z;
        break;

    case 'z':
        incidentPlaneCenter = incidentBox->transform->GetWorldPosition() + incidentPlaneDir * incidentBox->halfSize.z;
        oVec1 = incidentBox->transform->right * incidentBox->halfSize.x;
        oVec2 = incidentBox->transform->up * incidentBox->halfSize.y;
        break;

    default:
        std::cerr << "Invalid direction type";
        hit.point = refPlaneCenter;
        return;
    }
    
    incidentVertices.push_back(incidentPlaneCenter + oVec1 + oVec2);
    incidentVertices.push_back(incidentPlaneCenter + oVec1 - oVec2);
    incidentVertices.push_back(incidentPlaneCenter - oVec1 + oVec2);
    incidentVertices.push_back(incidentPlaneCenter - oVec1 - oVec2);

    // if ((referenceBox->transform->GetName() == "Box" && incidentBox->transform->GetName() == "Ground") ||
    //     (incidentBox->transform->GetName() == "Box" && referenceBox->transform->GetName() == "Ground"))
    // {
    //     std::cout << "Reference dir: " << referencePlaneDir << "\n";
    //     std::cout << "Incident vertices:\n";
    //     std::cout << "1: " << incidentVertices[0] << "\n";
    //     std::cout << "2: " << incidentVertices[1] << "\n";
    //     std::cout << "3: " << incidentVertices[2] << "\n";
    //     std::cout << "4: " << incidentVertices[3] << "\n";
    //     std::cout << "\n";
    // }

    // Warning: this is ALSO a crude approximation of the contact point, but (should be) MUCH more accurate than the last
    // Compute the average of the contact points
    std::vector<glm::vec3> contactPoints;
    glm::vec3 averagePoint(0.0f);

    for(const auto& incidentVertex : incidentVertices)
    {
        // Another approximation; check the commented code below
        if (referenceBox->CheckPointIsInside(incidentVertex))
        {
            contactPoints.push_back(incidentVertex);
            averagePoint += incidentVertex;
        }
        
        // // Calculate distance from vertex to the plane
        // float dist = glm::dot(refFaceWorldNormal, v) - planeDist;
        //
        // // This vertex is behind or on the reference plane
        // if (dist <= 0.0f)
        // {
        //     // Project the point onto the plane along the normal
        //     glm::vec3 contact = v - refFaceWorldNormal * dist;
        //     contactPoints.push_back(contact);
        //     averagePoint += contact;
        // }
    }

    if (contactPoints.empty())
    {
        if (useRetryFallback)
            CheckFaceFaceCollision(incidentBox, referenceBox, incidentPlaneCenter, hit, false);
        else
            hit.point = refPlaneCenter;
    } else
    {
        hit.point = averagePoint / static_cast<float>(contactPoints.size());
    }
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

    int currentOverlapIndex = -1;
    int minimumOverlapIndex = -1;
    float minimumOverlap = 1000000.0f;

    hit.normal = glm::vec3_up;
    hit.point = boxA->transform->GetWorldPosition();
    
    hit.hasHit = !(
        CheckSeparatingPlaneForBoxes(rPos, boxAX, boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, boxAY, boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, boxAZ, boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||

        CheckSeparatingPlaneForBoxes(rPos, boxBX, boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, boxBY, boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, boxBZ, boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAX, boxBX), boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAX, boxBY), boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAX, boxBZ), boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAY, boxBX), boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAY, boxBY), boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAY, boxBZ), boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAZ, boxBX), boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAZ, boxBY), boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex) ||
        CheckSeparatingPlaneForBoxes(rPos, glm::cross(boxAZ, boxBZ), boxA, boxB, hit, minimumOverlap, currentOverlapIndex, minimumOverlapIndex)
    );

    if (!hit.hasHit)
        return false;

    switch (minimumOverlapIndex)
    {
    case 0:
        CheckFaceFaceCollision(boxA, boxB, boxA->transform->GetWorldPosition() - hit.normal * boxA->halfSize.x, hit);
        break;
    case 1:
        CheckFaceFaceCollision(boxA, boxB, boxA->transform->GetWorldPosition() - hit.normal * boxA->halfSize.y, hit);
        break;
    case 2:
        CheckFaceFaceCollision(boxA, boxB, boxA->transform->GetWorldPosition() - hit.normal * boxA->halfSize.z, hit);
        break;
    case 3:
        CheckFaceFaceCollision(boxB, boxA, boxB->transform->GetWorldPosition() + hit.normal * boxB->halfSize.x, hit);
        break;
    case 4:
        CheckFaceFaceCollision(boxB, boxA, boxB->transform->GetWorldPosition() + hit.normal * boxB->halfSize.y, hit);
        break;
    case 5:
        CheckFaceFaceCollision(boxB, boxA, boxB->transform->GetWorldPosition() + hit.normal * boxB->halfSize.z, hit);
        break;
    case 6:
        CheckEdgeToEdgeCollision(boxA, 'x', boxB, 'x', hit.normal, hit);
        break;
    case 7:
        CheckEdgeToEdgeCollision(boxA, 'x', boxB, 'y', hit.normal, hit);
        break;
    case 8:
        CheckEdgeToEdgeCollision(boxA, 'x', boxB, 'z', hit.normal, hit);
        break;
    case 9:
        CheckEdgeToEdgeCollision(boxA, 'y', boxB, 'x', hit.normal, hit);
        break;
    case 10:
        CheckEdgeToEdgeCollision(boxA, 'y', boxB, 'y', hit.normal, hit);
        break;
    case 11:
        CheckEdgeToEdgeCollision(boxA, 'y', boxB, 'z', hit.normal, hit);
        break;
    case 12:
        CheckEdgeToEdgeCollision(boxA, 'z', boxB, 'x', hit.normal, hit);
        break;
    case 13:
        CheckEdgeToEdgeCollision(boxA, 'z', boxB, 'y', hit.normal, hit);
        break;
    case 14:
        CheckEdgeToEdgeCollision(boxA, 'z', boxB, 'z', hit.normal, hit);
        break;
    default:
        break;
    }

    return true;
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
        hit.normal = -glm::vec3_up;
    else
        hit.normal = -glm::normalize(hitNormal);

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

void BoxCollider::CloneToDevice(BoxCollider_Dev& boxCollider_d)
{
    boxCollider_d.transform.worldPosition = transform->GetWorldPosition();
    boxCollider_d.transform.right = transform->right;
    boxCollider_d.transform.up = transform->up;
    boxCollider_d.transform.forward = transform->forward;

    Rigidbody* rb = transform->GetComponent<Rigidbody>();
    boxCollider_d.rb.isAttached = rb != nullptr;
    if (rb != nullptr)
    {
        boxCollider_d.rb.isStatic = rb->IsStatic();
        boxCollider_d.rb.velocity = rb->GetVelocity();
        boxCollider_d.rb.angularVelocity = rb->GetAngularVelocity();
        boxCollider_d.rb.mass = rb->GetMass();
        boxCollider_d.rb.restitutionCoefficient = rb->GetRestitutionCoefficient();
        boxCollider_d.rb.momentOfInertia = GetMomentOfInertia(rb->GetMass());
    }

    boxCollider_d.halfSize = halfSize;
}
