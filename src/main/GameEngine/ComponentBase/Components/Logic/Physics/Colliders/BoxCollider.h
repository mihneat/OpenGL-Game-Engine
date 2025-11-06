#pragma once

#include <vector>
#include <unordered_set>

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
        BoxCollider(transform::Transform* transform) : Collider(transform) { }
        virtual ~BoxCollider() { }

        bool CollidesWith(Collider* other, CollisionHit& hit) override;
        
    protected:
        SERIALIZE_FIELD glm::vec3 halfSize = glm::vec3(.5f);

        // These methods are used for checking OBB collisions
        bool CheckBoxCollision(const BoxCollider* boxA, const BoxCollider* boxB, CollisionHit& hit) const;
        bool CheckSeparatingPlaneForBoxes(const glm::vec3& rPos, const glm::vec3& plane, const BoxCollider* boxA, const BoxCollider* boxB, CollisionHit& hit, float& minimumOverlap) const;

        bool CheckSphereCollision(const BoxCollider* box, const SphereCollider* sphere, CollisionHit& hit) const;
    };
}
