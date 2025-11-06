#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    SERIALIZE_CLASS
    class Rigidbody : public Component
    {
        MARK_SERIALIZABLE(Rigidbody)
        
    public:
        Rigidbody(transform::Transform* transform) : Component(transform) { }
        virtual ~Rigidbody() { }

        bool IsStatic() const;
        float GetMass() const;
        float GetRestitutionCoefficient() const;
        glm::vec3 GetVelocity() const;
        void SetVelocity(glm::vec3 velocity);
        
        void AddForce(glm::vec3 force);

    protected:
        SERIALIZE_FIELD float mass = 0.1f;
        
        SERIALIZE_FIELD glm::vec3 velocity = glm::vec3(0.0f);
        SERIALIZE_FIELD glm::vec3 angularVelocity = glm::vec3(0.0f);

        // Can be seen as bounciness; determines how elastic the collision is (how much will this object feel the impulse)
        // Should be between 0 and 1, but I have no way of clamping it in the GUI, so this should be fun
        SERIALIZE_FIELD float restitutionCoefficient = 0.8f;
        
        SERIALIZE_FIELD bool isStatic = false;
        
    };
}
