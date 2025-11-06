#include "Rigidbody.h"

bool component::Rigidbody::IsStatic() const
{
    return isStatic;
}

glm::vec3 component::Rigidbody::GetVelocity() const
{
    return velocity;
}

float component::Rigidbody::GetMass() const
{
    return mass;
}

float component::Rigidbody::GetRestitutionCoefficient() const
{
    return restitutionCoefficient;
}

void component::Rigidbody::SetVelocity(glm::vec3 newVelocity)
{
    velocity = newVelocity;
}

void component::Rigidbody::AddForce(glm::vec3 force)
{
    velocity += force;
}
