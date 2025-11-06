#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    struct CollisionHit
    {
        bool hasHit;
        glm::vec3 point;
        glm::vec3 normal;
    };
    
    SERIALIZE_CLASS
    class Collider : public Component
    {
        MARK_SERIALIZABLE(Collider)
        
    public:
        Collider(transform::Transform* transform) : Component(transform) { }
        virtual ~Collider() { }

        // TODO: This should be pure abstract, but the serializer doesn't allow it yet :)
        virtual bool CollidesWith(Collider* other, CollisionHit& hit);
        
    };
}
