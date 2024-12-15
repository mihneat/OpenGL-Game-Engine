#include "HelicopterMovement.h"

#include "core/engine.h"
#include "main/GameEngine/GameEngine.h"
#include "utils/memory_utils.h"

using namespace component;

void HelicopterMovement::Start()
{
    instancesParent = transform->GetTransformByTag("InstancesParent");
    modelTransform = transform->GetChild(0);
}

void HelicopterMovement::Update(float deltaTime)
{
    // Update the helicopter position uniform value
    defaultLitMaterial->UpdateVec3("helicopter_position", transform->GetWorldPosition());
    heightMapMaterial->UpdateVec3("helicopter_position", transform->GetWorldPosition());
    helicopterMaterial->UpdateVec3("helicopter_position", transform->GetWorldPosition());
    skyboxMaterial->UpdateVec3("helicopter_position", transform->GetWorldPosition());
    treeMaterial->UpdateVec3("helicopter_position", transform->GetWorldPosition());

    // Go to the destination
    if (!hasReachedDestination)
    {
        glm::vec3 posXoZ = transform->GetWorldPosition();
        posXoZ.y = 0.0f;
        
        glm::vec3 moveDirection = destination - posXoZ;
        if (length(moveDirection) > 0.2f)
        {
            glm::vec3 normalizedMoveDirection = glm::normalize(moveDirection);

            if (length(moveDirection) > glm::length(normalizedMoveDirection * speed * deltaTime))
                transform->Translate(normalizedMoveDirection * speed * deltaTime);
            else
                transform->Translate(moveDirection);

            float currAngle = transform->GetLocalRotation().y;
            transform->SetLocalRotation(glm::vec3(0, glm::mix(currAngle, targetAngle, turnSpeed * deltaTime), 0));
        } else
        {
            hasReachedDestination = true;
            if (spawnedMarker != nullptr)
            {
                transform::Transform::Destroy(spawnedMarker);
                spawnedMarker = nullptr;
            }
            
            targetTiltAngle = 0.0f;
        }
    }

    const float currTiltAngle = modelTransform->GetLocalRotation().z;
    const float targetAngleRads = glm::radians(targetTiltAngle);
    if (abs(currTiltAngle - targetAngleRads) > 0.001f)
        modelTransform->SetLocalRotation(glm::vec3(0, 0, glm::mix(currTiltAngle, targetAngleRads, glm::min(10.0f * deltaTime, 1.0f))));
}

void HelicopterMovement::MouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (!IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
        return;
    
    // Do a bit of hacking to get this working :)
    glm::vec4 selectionData = m1::GameEngine::ExtractSelectionData(mouseX, mouseY);
    std::cout << "New selection data: " << selectionData << "\n";

    // Ignore background clicks
    if (glm::length(glm::vec3(selectionData)) < 0.001f)
        return;

    // Spawn a marker at that position
    if (spawnedMarker != nullptr)
    {
        transform::Transform::Destroy(spawnedMarker);
        spawnedMarker = nullptr;
    }

    spawnedMarker = transform::Transform::Instantiate(prefabManager::PrefabManager::CreateMarker(instancesParent));
    spawnedMarker->Translate(glm::vec3(selectionData));

    // Set the helicopter state
    hasReachedDestination = false;
    destination = glm::vec3(selectionData);

    glm::vec3 posXoZ = transform->GetWorldPosition();
    posXoZ.y = 0.0f;
    
    glm::vec3 moveDirection = destination - posXoZ;
    moveDirection = glm::normalize(moveDirection);

    const float currAngle = transform->GetLocalRotation().y;
    // const float intervalStart = floor(currAngle / (glm::pi<float>() * 2.0f)) * glm::pi<float>() * 2.0f;
    
    const float targetAngle0 = glm::atan2(moveDirection.x, moveDirection.z) + glm::pi<float>() / 2.0f - glm::pi<float>() * 2.0f;
    const float targetAngle1 = glm::atan2(moveDirection.x, moveDirection.z) + glm::pi<float>() / 2.0f;
    const float targetAngle2 = glm::atan2(moveDirection.x, moveDirection.z) + glm::pi<float>() / 2.0f + glm::pi<float>() * 2.0f;

    // Choose the closer angle
    const float dist0 = abs(currAngle - targetAngle0);
    const float dist1 = abs(currAngle - targetAngle1);
    const float dist2 = abs(currAngle - targetAngle2);
    if (dist0 < dist1 && dist0 < dist2)
    {
        transform->Rotate(glm::vec3(0, glm::pi<float>() * 2.0f, 0));
        targetAngle = targetAngle1;
    } else if (dist1 < dist0 && dist1 < dist2) {
        // transform->Rotate(glm::vec3(0, glm::pi<float>() * 2.0f, 0));
        // currAngle += glm::pi<float>() * 2.0f;
        
        targetAngle = targetAngle1;
    } else
    {
        transform->Rotate(glm::vec3(0, -glm::pi<float>() * 2.0f, 0));
        targetAngle = targetAngle1;
    }

    targetTiltAngle = tiltAngle;
}
