#pragma once
#include <glm/vec3.hpp>

struct Transform_Dev
{
    int nameStartIndex;
    int nameLength;
    
    glm::vec3 worldPosition;

    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forward;
};

struct Rigidbody_Dev
{
    // This refers to the Rigidbody component being attached to an object with a collider
    bool isAttached;
    
    bool isStatic;
    
    glm::vec3 velocity;
    glm::vec3 angularVelocity;

    float mass;
    float restitutionCoefficient;
    float momentOfInertia;
};

struct CollisionHit_Dev
{
    bool hasHit;
    glm::vec3 point;
    glm::vec3 normal;
};
