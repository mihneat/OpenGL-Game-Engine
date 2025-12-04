#pragma once

#include <vector>
#include <unordered_set>

#include <cuda_runtime.h>

#include "BoxColliderGPU.cuh"
#include "SphereCollider.h"
#include "utils/glm_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Collider.h"

namespace component
{
    SERIALIZE_CLASS
    class BoxCollider : public Collider
    {
        MARK_SERIALIZABLE(BoxCollider)
        
    public:
        // Note: Box collider has the implementation of an OBB
        BoxCollider(transform::Transform* transform, glm::vec3 halfSize = glm::vec3(.5f)) : Collider(transform), halfSize(halfSize) { }
        virtual ~BoxCollider() { }

        bool CollidesWith(Collider* other, CollisionHit& hit) override;
        float GetMomentOfInertia(float mass) override;
        
        std::pair<glm::vec3, glm::vec3> GetClosestEdgeInDirectionOfAxis(char pDirChar, glm::vec3 axis, glm::vec3 otherBoxCenter) const;
        void CheckFaceFaceCollision(const BoxCollider* referenceBox, const BoxCollider* incidentBox, glm::vec3 refPlaneCenter, CollisionHit& hit, bool useRetryFallback = true) const;
        bool CheckPointIsInside(glm::vec3 point) const;

        void CloneToDevice(BoxCollider_Dev& boxCollider_d);
        
    protected:
        SERIALIZE_FIELD glm::vec3 halfSize = glm::vec3(.5f);

        // These methods are used for checking OBB collisions
        bool CheckBoxCollision(const BoxCollider* boxA, const BoxCollider* boxB, CollisionHit& hit) const;
        bool CheckSeparatingPlaneForBoxes(const glm::vec3& rPos, const glm::vec3& plane, const BoxCollider* boxA, const BoxCollider* boxB, CollisionHit& hit, float& minimumOverlap, int& currentOverlapIndex, int& minimumOverlapIndex) const;

        bool CheckSphereCollision(const BoxCollider* box, const SphereCollider* sphere, CollisionHit& hit) const;
    };
}
