#include "PhysicsEngine.h"

#include <iostream>

// Include CUDA
#include <cuda_runtime.h>

// Include helper functions
#include <helper_cuda.h>
#include <helper_functions.h>

#include "main/GameEngine/GameEngine.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Rigidbody.h"

using namespace physics;
using namespace component;

PhysicsEngine::~PhysicsEngine()
{
    FreeData();
}

void PhysicsEngine::ResolveCollision(Collider* colliderA, Collider* colliderB, CollisionHit& hit)
{
    Rigidbody* rbA = colliderA->transform->GetComponent<Rigidbody>();
    Rigidbody* rbB = colliderB->transform->GetComponent<Rigidbody>();
    if (rbA == nullptr || rbB == nullptr || rbA == rbB)
    {
        std::cout << "Either a Rigidbody is missing, or they are the same\n";
        return;
    }
    
    float massA = rbA->GetMass();
    float massB = rbB->GetMass();
    glm::vec3 velocityA = rbA->IsStatic() ? glm::vec3(0.0f) : rbA->GetVelocity();
    glm::vec3 velocityB = rbB->IsStatic() ? glm::vec3(0.0f) : rbB->GetVelocity();
    glm::vec3 relativeVelocity = velocityA - velocityB;
    
    // If relative normal velocity is negative, ignore the collision
    // Source: https://www.chrishecker.com/images/e/e7/Gdmphys3.pdf

    // TODO: This does not work perfectly, but I am hopeful that it can be fixed
    if (glm::dot(relativeVelocity, hit.normal) < 0.0f)
        return;

    // Check for resting contact
    float restitutionCoefficientA = rbA->GetRestitutionCoefficient();
    float restitutionCoefficientB = rbB->GetRestitutionCoefficient();
    float restitutionCoefficient = glm::min(restitutionCoefficientA, restitutionCoefficientB);
    
    // if (abs(glm::dot(relativeVelocity, hit.normal)) < 1.0f)
    //     restitutionCoefficient = 0.0f;

    // Compute the new linear velocities
    // Source 1: https://perso.liris.cnrs.fr/nicolas.pronost/UUCourses/GamePhysics/lectures/lecture%207%20Collision%20Resolution.pdf
    // Source 2: https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects
    float massFactor;
    if (rbA->IsStatic() && rbB->IsStatic())
        massFactor = 1.0f;
    else if (rbA->IsStatic())
        massFactor = 1.0f / massB;
    else if (rbB->IsStatic())
        massFactor = 1.0f / massA;
    else
        massFactor = (massA + massB) / (massA * massB);
    
    float denominator = massFactor;
    
    float impulseMagnitude = -(1.0f + restitutionCoefficient) * glm::dot(relativeVelocity, hit.normal) / denominator;
    
    glm::vec3 impulse = hit.normal * impulseMagnitude;

    glm::vec3 newLinearVelocityA = rbA->GetVelocity() + impulse / massA;
    glm::vec3 newLinearVelocityB = rbB->GetVelocity() - impulse / massB;
    
    rbA->SetVelocity(rbA->IsStatic() ? glm::vec3(0.0f) : newLinearVelocityA);
    rbB->SetVelocity(rbB->IsStatic() ? glm::vec3(0.0f) : newLinearVelocityB);
}

void PhysicsEngine::ResolveCollisionWithFriction(Collider* colliderA, Collider* colliderB, CollisionHit& hit)
{
    Rigidbody* rbA = colliderA->transform->GetComponent<Rigidbody>();
    Rigidbody* rbB = colliderB->transform->GetComponent<Rigidbody>();
    if (rbA == nullptr || rbB == nullptr || rbA == rbB)
    {
        std::cout << "Either a Rigidbody is missing, or they are the same\n";
        return;
    }
    
    float massA = rbA->GetMass();
    float massB = rbB->GetMass();
    glm::vec3 velocityA = rbA->IsStatic() ? glm::vec3(0.0f) : rbA->GetVelocity();
    glm::vec3 velocityB = rbB->IsStatic() ? glm::vec3(0.0f) : rbB->GetVelocity();
    glm::vec3 relativeVelocity = velocityA - velocityB;
    
    // If relative normal velocity is negative, ignore the collision
    // Source: https://www.chrishecker.com/images/e/e7/Gdmphys3.pdf

    // TODO: This does not work perfectly, but I am hopeful that it can be fixed
    if (glm::dot(relativeVelocity, hit.normal) < 0.0f)
        return;

    // Check for resting contact
    float restitutionCoefficientA = rbA->GetRestitutionCoefficient();
    float restitutionCoefficientB = rbB->GetRestitutionCoefficient();
    float restitutionCoefficient = glm::min(restitutionCoefficientA, restitutionCoefficientB);
    
    if (abs(glm::dot(relativeVelocity, hit.normal)) < 1.0f)
        restitutionCoefficient = 0.0f;

    // Compute the new linear velocities
    // Source 1: https://perso.liris.cnrs.fr/nicolas.pronost/UUCourses/GamePhysics/lectures/lecture%207%20Collision%20Resolution.pdf
    // Source 2: https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects
    float massFactor;
    if (rbA->IsStatic() && rbB->IsStatic())
        massFactor = 1.0f;
    else if (rbA->IsStatic())
        massFactor = 1.0f / massB;
    else if (rbB->IsStatic())
        massFactor = 1.0f / massA;
    else
        massFactor = (massA + massB) / (massA * massB);
    
    float denominator = massFactor;
    
    float impulseMagnitude = -(1.0f + restitutionCoefficient) * glm::dot(relativeVelocity, hit.normal) / denominator;
    glm::vec3 impulse = hit.normal * impulseMagnitude;

    // Compute the friction force
    // Thanks to: https://www.youtube.com/watch?v=YpwLArBofkk
    glm::vec3 tangent = relativeVelocity - glm::dot(relativeVelocity, hit.normal) * hit.normal; // glm::normalize(glm::cross(glm::cross(hit.normal, relativeVelocity), hit.normal));
    if (glm::length(tangent) < 0.001f)
        tangent = glm::vec3(0);
    else
        tangent = glm::normalize(tangent);
    
    float staticFrictionCoefficient = 0.4f;
    float dynamicFrictionCoefficient = 0.1f;
    
    float tangent_impulseMagnitude = -glm::dot(relativeVelocity, tangent) / denominator;
    glm::vec3 frictionImpulse;
    if (abs(tangent_impulseMagnitude) <= abs(impulseMagnitude) * staticFrictionCoefficient)
        frictionImpulse = tangent_impulseMagnitude * tangent * 0.1f;
    else
        frictionImpulse = impulseMagnitude * tangent * dynamicFrictionCoefficient;
    
    glm::vec3 totalImpulse = impulse + frictionImpulse;

    glm::vec3 newLinearVelocityA = rbA->GetVelocity() + totalImpulse / massA;
    glm::vec3 newLinearVelocityB = rbB->GetVelocity() - totalImpulse / massB;
    
    rbA->SetVelocity(rbA->IsStatic() ? glm::vec3(0.0f) : newLinearVelocityA);
    rbB->SetVelocity(rbB->IsStatic() ? glm::vec3(0.0f) : newLinearVelocityB);
}

void PhysicsEngine::ResolveCollisionWithRotation(Collider* colliderA, Collider* colliderB, CollisionHit& hit)
{
    Rigidbody* rbA = colliderA->transform->GetComponent<Rigidbody>();
    Rigidbody* rbB = colliderB->transform->GetComponent<Rigidbody>();
    if (rbA == nullptr || rbB == nullptr || rbA == rbB)
    {
        std::cout << "Either a Rigidbody is missing, or they are the same\n";
        return;
    }
    
    float massA = rbA->GetMass();
    float massB = rbB->GetMass();
    glm::vec3 vecToHitPointA = hit.point - colliderA->transform->GetWorldPosition();
    glm::vec3 vecToHitPointB = hit.point - colliderB->transform->GetWorldPosition();
    glm::vec3 velocityA = rbA->IsStatic() ? glm::vec3(0.0f) : rbA->GetVelocity() + glm::cross(rbA->GetAngularVelocity(), vecToHitPointA);
    glm::vec3 velocityB = rbB->IsStatic() ? glm::vec3(0.0f) : rbB->GetVelocity() + glm::cross(rbB->GetAngularVelocity(), vecToHitPointB);
    glm::vec3 relativeVelocity = velocityA - velocityB;
    
    // If relative normal velocity is negative, ignore the collision
    // Source: https://www.chrishecker.com/images/e/e7/Gdmphys3.pdf

    // TODO: This does not work perfectly, but I am hopeful that it can be fixed
    if (glm::dot(relativeVelocity, hit.normal) < 0.0f)
        return;

    // Check for resting contact
    float restitutionCoefficientA = rbA->GetRestitutionCoefficient();
    float restitutionCoefficientB = rbB->GetRestitutionCoefficient();
    float restitutionCoefficient = glm::min(restitutionCoefficientA, restitutionCoefficientB);
    
    if (abs(glm::dot(relativeVelocity, hit.normal)) < 1.0f)
        restitutionCoefficient = 0.0f;

    // Compute the new linear velocities
    // Source 1: https://perso.liris.cnrs.fr/nicolas.pronost/UUCourses/GamePhysics/lectures/lecture%207%20Collision%20Resolution.pdf
    // Source 2: https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects
    float massFactor;
    if (rbA->IsStatic() && rbB->IsStatic())
        massFactor = 1.0f;
    else if (rbA->IsStatic())
        massFactor = 1.0f / massB;
    else if (rbB->IsStatic())
        massFactor = 1.0f / massA;
    else
        massFactor = (massA + massB) / (massA * massB);

    float momentOfInertiaInverseA = 0.1f / colliderA->GetMomentOfInertia(rbA->GetMass());
    float momentOfInertiaInverseB = 0.1f / colliderB->GetMomentOfInertia(rbB->GetMass());
    float angularVelocityTerm = glm::dot(
        (rbA->IsStatic() ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseA * glm::cross(vecToHitPointA, hit.normal), vecToHitPointA)) +
        (rbB->IsStatic() ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseB * glm::cross(vecToHitPointB, hit.normal), vecToHitPointB)),
        hit.normal
    );
    float denominator = massFactor + angularVelocityTerm;
    
    float impulseMagnitude = -(1.0f + restitutionCoefficient) * glm::dot(relativeVelocity, hit.normal) / denominator;
    glm::vec3 impulse = hit.normal * impulseMagnitude;

    glm::vec3 newLinearVelocityA = rbA->GetVelocity() + impulse / massA;
    glm::vec3 newLinearVelocityB = rbB->GetVelocity() - impulse / massB;
    
    glm::vec3 newAngularVelocityA = rbA->GetAngularVelocity() + momentOfInertiaInverseA * glm::cross(vecToHitPointA, impulse);
    glm::vec3 newAngularVelocityB = rbB->GetAngularVelocity() - momentOfInertiaInverseB * glm::cross(vecToHitPointB, impulse);
    
    rbA->SetVelocity(rbA->IsStatic() ? glm::vec3(0.0f) : newLinearVelocityA);
    rbB->SetVelocity(rbB->IsStatic() ? glm::vec3(0.0f) : newLinearVelocityB);
    
    rbA->SetAngularVelocity(rbA->IsStatic() ? glm::vec3(0.0f) : newAngularVelocityA);
    rbB->SetAngularVelocity(rbB->IsStatic() ? glm::vec3(0.0f) : newAngularVelocityB);
}

void PhysicsEngine::ResolveCollisionWithRotationAndFriction(Collider* colliderA, Collider* colliderB, CollisionHit& hit)
{
    Rigidbody* rbA = colliderA->transform->GetComponent<Rigidbody>();
    Rigidbody* rbB = colliderB->transform->GetComponent<Rigidbody>();
    if (rbA == nullptr || rbB == nullptr || rbA == rbB)
    {
        std::cout << "Either a Rigidbody is missing, or they are the same\n";
        return;
    }
    
    float massA = rbA->GetMass();
    float massB = rbB->GetMass();
    glm::vec3 vecToHitPointA = hit.point - colliderA->transform->GetWorldPosition();
    glm::vec3 vecToHitPointB = hit.point - colliderB->transform->GetWorldPosition();
    glm::vec3 velocityA = rbA->IsStatic() ? glm::vec3(0.0f) : rbA->GetVelocity() + glm::cross(rbA->GetAngularVelocity(), vecToHitPointA);
    glm::vec3 velocityB = rbB->IsStatic() ? glm::vec3(0.0f) : rbB->GetVelocity() + glm::cross(rbB->GetAngularVelocity(), vecToHitPointB);
    glm::vec3 relativeVelocity = velocityA - velocityB;

    // if ((colliderA->transform->GetName() == "Box" && colliderB->transform->GetName() == "Ground") ||
    //     (colliderB->transform->GetName() == "Box" && colliderA->transform->GetName() == "Ground"))
    // {
    //     std::cout << "=============================================================================================================\n";
    //     std::cout << colliderA->transform->GetName() << ": " << velocityA << ", " << colliderB->transform->GetName() << ": " << velocityB << '\n';
    //     std::cout << "Relative velocity: " << relativeVelocity << '\n';
    //     std::cout << "Hit point: " << hit.point << '\n';
    //     std::cout << "Hit normal: " << hit.normal << '\n';
    //     std::cout << "Hit normal tip: " << hit.point + hit.normal << '\n';
    //     std::cout << "Dot product: " << glm::dot(relativeVelocity, hit.normal) << '\n';
    //     std::cout << "\n";
    // }

    // If relative normal velocity is negative, ignore the collision
    // Source: https://www.chrishecker.com/images/e/e7/Gdmphys3.pdf
    if (glm::dot(relativeVelocity, hit.normal) > 0.0f)
        return;

    // Check for resting contact
    float restitutionCoefficientA = rbA->GetRestitutionCoefficient();
    float restitutionCoefficientB = rbB->GetRestitutionCoefficient();
    float restitutionCoefficient = glm::min(restitutionCoefficientA, restitutionCoefficientB);
    
    if (glm::length(relativeVelocity) < 0.0001f)
        restitutionCoefficient = 0.0f;

    // Compute the impulse magnitude
    // Source 1: https://perso.liris.cnrs.fr/nicolas.pronost/UUCourses/GamePhysics/lectures/lecture%207%20Collision%20Resolution.pdf
    // Source 2: https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects
    float massFactor;
    if (rbA->IsStatic() && rbB->IsStatic())
        massFactor = 1.0f;
    else if (rbA->IsStatic())
        massFactor = 1.0f / massB;
    else if (rbB->IsStatic())
        massFactor = 1.0f / massA;
    else
        massFactor = (massA + massB) / (massA * massB);

    float momentOfInertiaInverseA = 0.1f / colliderA->GetMomentOfInertia(rbA->GetMass());
    float momentOfInertiaInverseB = 0.1f / colliderB->GetMomentOfInertia(rbB->GetMass());
    float angularVelocityTerm = glm::dot(
        (rbA->IsStatic() ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseA * glm::cross(vecToHitPointA, hit.normal), vecToHitPointA)) +
        (rbB->IsStatic() ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseB * glm::cross(vecToHitPointB, hit.normal), vecToHitPointB)),
        hit.normal
    );
    float denominator = massFactor + angularVelocityTerm;
    
    float impulseMagnitude = -(1.0f + restitutionCoefficient) * glm::dot(relativeVelocity, hit.normal) / denominator;
    glm::vec3 impulse = hit.normal * impulseMagnitude;

    // Compute the friction force
    // Thanks to: https://www.youtube.com/watch?v=YpwLArBofkk
    glm::vec3 tangent = relativeVelocity - glm::dot(relativeVelocity, hit.normal) * hit.normal; // glm::normalize(glm::cross(glm::cross(hit.normal, relativeVelocity), hit.normal));
    if (glm::length(tangent) < 0.001f)
        tangent = glm::vec3(0);
    else
        tangent = -glm::normalize(tangent);
    
    float staticFrictionCoefficient = 0.4f;
    float dynamicFrictionCoefficient = 0.1f;

    float tangent_angularVelocityTerm = glm::dot(
        (rbA->IsStatic() ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseA * glm::cross(vecToHitPointA, tangent), vecToHitPointA)) +
        (rbB->IsStatic() ? glm::vec3(0.0f) : glm::cross(momentOfInertiaInverseB * glm::cross(vecToHitPointB, tangent), vecToHitPointB)),
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

    glm::vec3 newLinearVelocityA = rbA->GetVelocity() + totalImpulse / massA;
    glm::vec3 newLinearVelocityB = rbB->GetVelocity() - totalImpulse / massB;
    
    glm::vec3 newAngularVelocityA = rbA->GetAngularVelocity() + momentOfInertiaInverseA * glm::cross(vecToHitPointA, totalImpulse);
    glm::vec3 newAngularVelocityB = rbB->GetAngularVelocity() - momentOfInertiaInverseB * glm::cross(vecToHitPointB, totalImpulse);
    
    rbA->SetVelocity(rbA->IsStatic() ? glm::vec3(0.0f) : newLinearVelocityA);
    rbB->SetVelocity(rbB->IsStatic() ? glm::vec3(0.0f) : newLinearVelocityB);
    
    rbA->SetAngularVelocity(rbA->IsStatic() ? glm::vec3(0.0f) : newAngularVelocityA);
    rbB->SetAngularVelocity(rbB->IsStatic() ? glm::vec3(0.0f) : newAngularVelocityB);
}

void PhysicsEngine::SimulatePhysicsCPU(transform::Transform* transform, const float deltaTime)
{
    static constexpr float g = 9.81f;
    
    // Find all Colliders
    std::vector<Collider*> colliders;
    std::vector<Rigidbody*> rbs;
    m1::GameEngine::ApplyToComponents(transform, [&colliders, &rbs](Component* component) {
        Collider* collider = dynamic_cast<Collider*>(component);
        Rigidbody* rb = dynamic_cast<Rigidbody*>(component);
        
        if (collider != nullptr && collider->IsActive())
            colliders.push_back(collider);
        
        if (rb != nullptr && rb->IsActive())
            rbs.push_back(rb);
    });

    // Apply external forces
    for (auto rb : rbs)
    {
        if (rb->IsStatic())
            continue;

        // Apply external forces
        float G = rb->GetMass() * g;
        rb->AddForce(glm::vec3_down * G * deltaTime);
    }

    // Predict one step
    std::vector<glm::vec3> prevVelocities;
    std::vector<glm::vec3> prevAngularVelocities;
    for (auto rb : rbs)
    {
        prevVelocities.push_back(rb->GetVelocity());
        prevAngularVelocities.push_back(rb->GetAngularVelocity());
        
        if (rb->IsStatic())
            continue;

        // Apply physics update
        rb->transform->Translate(rb->GetVelocity() * deltaTime);

        // Predict angular velocity
        rb->transform->Rotate(-rb->GetAngularVelocity() * deltaTime);
    }
    
    // Resolve collisions
    for (int i = 0; i < colliders.size(); ++i)
    {
        Collider* colliderA = colliders[i];
        for (int j = i + 1; j < colliders.size(); ++j)
        {
            Collider* colliderB = colliders[j];
            
            // Detect and resolve collision
            CollisionHit hit;
            hit.hasHit = false;
            
            if (colliderA->CollidesWith(colliderB, hit))
                ResolveCollisionWithRotationAndFriction(colliderA, colliderB, hit);
        }
    }
    
    // Revert prediction
    for (int i = 0; i < rbs.size(); ++i)
    {
        Rigidbody* rb = rbs[i];
        if (rb->IsStatic())
            continue;

        // Revert physics update
        rb->transform->Translate(-prevVelocities[i] * deltaTime);

        // Revert angular velocity
        rb->transform->Rotate(prevAngularVelocities[i] * deltaTime);
    }
    
    // Apply physics update
    for (int i = 0; i < rbs.size(); ++i)
    {
        Rigidbody* rb = rbs[i];
        if (rb->IsStatic())
            continue;

        // Apply physics update
        rb->transform->Translate(rb->GetVelocity() * deltaTime);

        // Apply angular velocity
        rb->transform->Rotate(-rb->GetAngularVelocity() * deltaTime);
    }
}

inline void PhysicsEngine::CopyDataHostToDevice(const std::vector<BoxCollider*>& boxColliders, const std::vector<SphereCollider*>& sphereColliders)
{
    int boxCnt = static_cast<int>(boxColliders.size());
    int sphereCnt = static_cast<int>(sphereColliders.size());
    if (boxCnt != prevBoxCnt || sphereCnt != prevSphereCnt)
    {
        prevBoxCnt = boxCnt;
        prevSphereCnt = sphereCnt;
        
        // Reallocate the device memory
        FreeData();

        checkCudaErrors(cudaMalloc(reinterpret_cast<void**>(&hit_d), 1 * sizeof(CollisionHit_Dev)));
        checkCudaErrors(cudaMalloc(reinterpret_cast<void**>(&boxColliders_d), boxCnt * sizeof(BoxCollider_Dev)));
        checkCudaErrors(cudaMalloc(reinterpret_cast<void**>(&sphereColliders_d), sphereCnt * sizeof(SphereCollider_Dev)));

        boxColliders_h = static_cast<BoxCollider_Dev*>(malloc(boxCnt * sizeof(BoxCollider_Dev)));
        sphereColliders_h = static_cast<SphereCollider_Dev*>(malloc(sphereCnt * sizeof(SphereCollider_Dev)));
    }
    
    checkCudaErrors(cudaMemset(hit_d, 0x0, 1 * sizeof(CollisionHit_Dev)));
    checkCudaErrors(cudaMemset(boxColliders_d, 0x0, boxCnt * sizeof(BoxCollider_Dev)));
    checkCudaErrors(cudaMemset(sphereColliders_d, 0x0, sphereCnt * sizeof(SphereCollider_Dev)));

    for (int i = 0; i < boxCnt; ++i)
        boxColliders[i]->CloneToDevice(boxColliders_h[i]);

    for (int i = 0; i < sphereCnt; ++i)
        sphereColliders[i]->CloneToDevice(sphereColliders_h[i]);
    
	checkCudaErrors(cudaMemcpy(boxColliders_d, boxColliders_h, boxCnt * sizeof(BoxCollider_Dev), cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(sphereColliders_d, sphereColliders_h, sphereCnt * sizeof(SphereCollider_Dev), cudaMemcpyHostToDevice));
}

inline void PhysicsEngine::CopyDataDeviceToHost(std::vector<BoxCollider*>& boxColliders, std::vector<SphereCollider*>& sphereColliders)
{
    int boxCnt = static_cast<int>(boxColliders.size());
    int sphereCnt = static_cast<int>(sphereColliders.size());
    checkCudaErrors(cudaMemcpy(boxColliders_h, boxColliders_d, boxCnt * sizeof(BoxCollider_Dev), cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy(sphereColliders_h, sphereColliders_d, sphereCnt * sizeof(SphereCollider_Dev), cudaMemcpyDeviceToHost));

    // TODO: This is quite inefficient.. getting the rigidbody every single time
    
    // Update the velocities
    for (int i = 0; i < boxCnt; ++i)
    {
        Rigidbody* rb = boxColliders[i]->transform->GetComponent<Rigidbody>();
        if (!rb)
            continue;

        rb->SetVelocity(boxColliders_h[i].rb.velocity);
        rb->SetAngularVelocity(boxColliders_h[i].rb.angularVelocity);
    }

    for (int i = 0; i < sphereCnt; ++i)
    {
        Rigidbody* rb = sphereColliders[i]->transform->GetComponent<Rigidbody>();
        if (!rb)
            continue;

        rb->SetVelocity(sphereColliders_h[i].rb.velocity);
        rb->SetAngularVelocity(sphereColliders_h[i].rb.angularVelocity);
    }
}

inline void PhysicsEngine::FreeData()
{
    if (hit_d != nullptr)
    {
        checkCudaErrors(cudaFree(hit_d));
        hit_d = nullptr;
    }
    
    if (boxColliders_d != nullptr)
    {
        checkCudaErrors(cudaFree(boxColliders_d));
        boxColliders_d = nullptr;
    }
    
    if (sphereColliders_d != nullptr)
    {
        checkCudaErrors(cudaFree(sphereColliders_d));
        sphereColliders_d = nullptr;
    }
    
    if (boxColliders_h != nullptr)
    {
        free(boxColliders_h);
        boxColliders_h = nullptr;
    }
    
    if (sphereColliders_h != nullptr)
    {
        free(sphereColliders_h);
        sphereColliders_h = nullptr;
    }
}

void PhysicsEngine::SimulatePhysicsGPU(transform::Transform* transform, const float deltaTime)
{
    int gpuBlockSize = 16;

    static constexpr float g = 9.81f;
    
    // Find all Colliders
    std::vector<Collider*> colliders;
    std::vector<BoxCollider*> boxColliders;
    std::vector<SphereCollider*> sphereColliders;
    std::vector<Rigidbody*> rbs;
    m1::GameEngine::ApplyToComponents(transform, [&colliders, &rbs, &boxColliders, &sphereColliders](Component* component) {
        Collider* collider = dynamic_cast<Collider*>(component);
        Rigidbody* rb = dynamic_cast<Rigidbody*>(component);
        
        if (collider != nullptr && collider->IsActive())
        {
            colliders.push_back(collider);
            if (dynamic_cast<BoxCollider*>(collider) != nullptr)
                boxColliders.push_back(dynamic_cast<BoxCollider*>(collider));
            else if (dynamic_cast<SphereCollider*>(collider) != nullptr)
                sphereColliders.push_back(dynamic_cast<SphereCollider*>(collider));
        }
        
        if (rb != nullptr && rb->IsActive())
            rbs.push_back(rb);
    });

    // Apply external forces
    for (auto rb : rbs)
    {
        if (rb->IsStatic())
            continue;

        // Apply external forces
        float G = rb->GetMass() * g;
        rb->AddForce(glm::vec3_down * G * deltaTime);
    }

    // Predict one step
    std::vector<glm::vec3> prevVelocities;
    std::vector<glm::vec3> prevAngularVelocities;
    for (auto rb : rbs)
    {
        prevVelocities.push_back(rb->GetVelocity());
        prevAngularVelocities.push_back(rb->GetAngularVelocity());
        
        if (rb->IsStatic())
            continue;

        // Apply physics update
        rb->transform->Translate(rb->GetVelocity() * deltaTime);

        // Predict angular velocity
        rb->transform->Rotate(-rb->GetAngularVelocity() * deltaTime);
    }
    
    // Allocate memory on the GPU
    dim3 dimBlock(gpuBlockSize, 1);
    dim3 dimGrid((static_cast<int>(colliders.size()) + gpuBlockSize - 1) / dimBlock.x, 1);

    CopyDataHostToDevice(boxColliders, sphereColliders);
    
    checkCudaErrors(ProcessCollisionsOnGPU(hit_d, boxColliders_d, sphereColliders_d, boxColliders.size(), sphereColliders.size(), dimGrid, dimBlock));
    checkCudaErrors(cudaDeviceSynchronize());
    
    CopyDataDeviceToHost(boxColliders, sphereColliders);
    
    // Revert prediction
    for (int i = 0; i < rbs.size(); ++i)
    {
        Rigidbody* rb = rbs[i];
        if (rb->IsStatic())
            continue;

        // Revert physics update
        rb->transform->Translate(-prevVelocities[i] * deltaTime);

        // Revert angular velocity
        rb->transform->Rotate(prevAngularVelocities[i] * deltaTime);
    }
    
    // Apply physics update
    for (int i = 0; i < rbs.size(); ++i)
    {
        Rigidbody* rb = rbs[i];
        if (rb->IsStatic())
            continue;

        // Apply physics update
        rb->transform->Translate(rb->GetVelocity() * deltaTime);

        // Apply angular velocity
        rb->transform->Rotate(-rb->GetAngularVelocity() * deltaTime);
    }
}

void PhysicsEngine::SimulatePhysics(transform::Transform* transform, const float deltaTime)
{
    if (useGPUAcceleration)
        SimulatePhysicsGPU(transform, deltaTime);
    else
        SimulatePhysicsCPU(transform, deltaTime);
}
