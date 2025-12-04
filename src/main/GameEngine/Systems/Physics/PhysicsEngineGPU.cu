#include "PhysicsEngineGPU.cuh"

#include <glm/ext/quaternion_geometric.hpp>

__device__ inline void k_ResolveCollisionWithRotationAndFriction(
    Rigidbody_Dev* rbA, Rigidbody_Dev* rbB,
    Transform_Dev* transformA, Transform_Dev* transformB,
    CollisionHit_Dev* hit
)
{
    if (!rbA->isAttached || !rbB->isAttached) // TODO: Untreated case on GPU:  rbA == rbB
        return;
    
    float massA = rbA->mass;
    float massB = rbB->mass;
    glm::vec3 vecToHitPointA = hit->point - transformA->worldPosition;
    glm::vec3 vecToHitPointB = hit->point - transformB->worldPosition;
    glm::vec3 velocityA = rbA->isStatic ? glm::vec3(0.0f) : rbA->velocity + glm::cross(rbA->angularVelocity, vecToHitPointA);
    glm::vec3 velocityB = rbB->isStatic ? glm::vec3(0.0f) : rbB->velocity + glm::cross(rbB->angularVelocity, vecToHitPointB);
    glm::vec3 relativeVelocity = velocityA - velocityB;
    
    // If relative normal velocity is negative, ignore the collision
    // Source: https://www.chrishecker.com/images/e/e7/Gdmphys3.pdf
    if (glm::dot(relativeVelocity, hit->normal) > 0.0f)
        return;

    // Check for resting contact
    float restitutionCoefficientA = rbA->restitutionCoefficient;
    float restitutionCoefficientB = rbB->restitutionCoefficient;
    float restitutionCoefficient = glm::min(restitutionCoefficientA, restitutionCoefficientB);
    
    if (glm::length(relativeVelocity) < 0.0001f)
        restitutionCoefficient = 0.0f;

    // Compute the impulse magnitude
    // Source 1: https://perso.liris.cnrs.fr/nicolas.pronost/UUCourses/GamePhysics/lectures/lecture%207%20Collision%20Resolution.pdf
    // Source 2: https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects
    float massFactor;
    if (rbA->isStatic && rbB->isStatic)
        massFactor = 1.0f;
    else if (rbA->isStatic)
        massFactor = 1.0f / massB;
    else if (rbB->isStatic)
        massFactor = 1.0f / massA;
    else
        massFactor = (massA + massB) / (massA * massB);

    float momentOfInertiaInverseA = 0.1f / rbA->momentOfInertia;
    float momentOfInertiaInverseB = 0.1f / rbB->momentOfInertia;
    float angularVelocityTerm = glm::dot(
        (rbA->isStatic ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseA * glm::cross(vecToHitPointA, hit->normal), vecToHitPointA)) +
        (rbB->isStatic ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseB * glm::cross(vecToHitPointB, hit->normal), vecToHitPointB)),
        hit->normal
    );
    float denominator = massFactor + angularVelocityTerm;
    
    float impulseMagnitude = -(1.0f + restitutionCoefficient) * glm::dot(relativeVelocity, hit->normal) / denominator;
    glm::vec3 impulse = hit->normal * impulseMagnitude;

    // Compute the friction force
    // Thanks to: https://www.youtube.com/watch?v=YpwLArBofkk
    glm::vec3 tangent = relativeVelocity - glm::dot(relativeVelocity, hit->normal) * hit->normal;
    if (glm::length(tangent) < 0.001f)
        tangent = glm::vec3(0);
    else
        tangent = -glm::normalize(tangent);
    
    float staticFrictionCoefficient = 0.4f;
    float dynamicFrictionCoefficient = 0.1f;

    float tangent_angularVelocityTerm = glm::dot(
        (rbA->isStatic ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseA * glm::cross(vecToHitPointA, tangent), vecToHitPointA)) +
        (rbB->isStatic ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseB * glm::cross(vecToHitPointB, tangent), vecToHitPointB)),
        tangent
    );
    float tangent_denominator = massFactor + tangent_angularVelocityTerm;
    float tangent_impulseMagnitude = -glm::dot(relativeVelocity, tangent) / tangent_denominator;
    
    // Check Coulomb's law
    glm::vec3 frictionImpulse;
    if (abs(tangent_impulseMagnitude) <= impulseMagnitude * staticFrictionCoefficient)
        frictionImpulse = tangent_impulseMagnitude * tangent * 0.1f;
    else
        frictionImpulse = impulseMagnitude * tangent * dynamicFrictionCoefficient;

    // Get the total impulse
    glm::vec3 totalImpulse = impulse + frictionImpulse;

    glm::vec3 newLinearVelocityA = rbA->velocity + totalImpulse / massA;
    glm::vec3 newLinearVelocityB = rbB->velocity - totalImpulse / massB;
    
    glm::vec3 newAngularVelocityA = rbA->angularVelocity + momentOfInertiaInverseA * glm::cross(vecToHitPointA, totalImpulse);
    glm::vec3 newAngularVelocityB = rbB->angularVelocity - momentOfInertiaInverseB * glm::cross(vecToHitPointB, totalImpulse);
    
    rbA->velocity = rbA->isStatic ? glm::vec3(0.0f) : newLinearVelocityA;
    rbB->velocity = rbB->isStatic ? glm::vec3(0.0f) : newLinearVelocityB;
    
    rbA->angularVelocity = rbA->isStatic ? glm::vec3(0.0f) : newAngularVelocityA;
    rbB->angularVelocity = rbB->isStatic ? glm::vec3(0.0f) : newAngularVelocityB;
}

__device__ inline void k_ProcessBoxCollisions(CollisionHit_Dev* hit, BoxCollider_Dev* boxColliders_d, SphereCollider_Dev* sphereColliders_d, int boxCnt, int sphereCnt)
{
    const int idx = blockIdx.x * blockDim.x + threadIdx.x;
    BoxCollider_Dev* currBoxCollider = &boxColliders_d[idx];
    
    // Iterate through all the boxes
    for (int i = idx + 1; i < boxCnt; ++i)
    {
        hit->hasHit = false;
        if (currBoxCollider->k_CollidesWithBox(&boxColliders_d[i], hit))
            k_ResolveCollisionWithRotationAndFriction(
                &currBoxCollider->rb, &boxColliders_d[i].rb,
                &currBoxCollider->transform, &boxColliders_d[i].transform,
                hit
            );
    }
    
    // Iterate through all the spheres
    for (int i = 0; i < sphereCnt; ++i)
    {
        hit->hasHit = false;
        if (currBoxCollider->k_CollidesWithSphere(&sphereColliders_d[i], hit))
            k_ResolveCollisionWithRotationAndFriction(
                &currBoxCollider->rb, &sphereColliders_d[i].rb,
                &currBoxCollider->transform, &sphereColliders_d[i].transform,
                hit
            );
    }
}

__device__ inline void k_ProcessSphereCollisions(CollisionHit_Dev* hit, SphereCollider_Dev* sphereColliders_d, int boxCnt, int sphereCnt)
{
    const int idx = blockIdx.x * blockDim.x + threadIdx.x;
    SphereCollider_Dev* currSphereCollider = &sphereColliders_d[idx - boxCnt];
    
    // Iterate through all the spheres
    for (int i = idx - boxCnt + 1; i < sphereCnt; ++i)
    {
        hit->hasHit = false;
        if (currSphereCollider->k_CollidesWithSphere(&sphereColliders_d[i], hit))
            k_ResolveCollisionWithRotationAndFriction(
                &currSphereCollider->rb, &sphereColliders_d[i].rb,
                &currSphereCollider->transform, &sphereColliders_d[i].transform,
                hit
            );
    }
}

// Kernel executed on CUDA device
__global__ void k_ProcessCollisions(CollisionHit_Dev* hit, BoxCollider_Dev* boxColliders_d, SphereCollider_Dev* sphereColliders_d, int boxCnt, int sphereCnt)
{
    const int idx = blockIdx.x * blockDim.x + threadIdx.x;
    const int n = boxCnt + sphereCnt;

    if (idx >= n)
        return;

    if (idx < boxCnt)
        k_ProcessBoxCollisions(hit, boxColliders_d, sphereColliders_d, boxCnt, sphereCnt);
    else
        k_ProcessSphereCollisions(hit, sphereColliders_d, boxCnt, sphereCnt);
        
}

cudaError_t ProcessCollisionsOnGPU(CollisionHit_Dev* hit, BoxCollider_Dev* boxColliders_d, SphereCollider_Dev* sphereColliders_d,
    int boxCnt, int sphereCnt, dim3 DIM_GRID, dim3 DIM_BLOCK)
{
    k_ProcessCollisions <<< DIM_GRID, DIM_BLOCK >>> (hit, boxColliders_d, sphereColliders_d, boxCnt, sphereCnt);

    return cudaGetLastError();
}
