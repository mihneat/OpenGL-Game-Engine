#include "main/GameEngine/ComponentBase/Components/Logic/Objects/Obstacle.h"

#include <iostream>

#include "main/GameEngine/Managers/GameInstance.h"

using namespace std;
using namespace component;
using namespace component;
using namespace transform;
using namespace prefabManager;

void Obstacle::Awake()
{
    // Get the game manager
    gameManager = managers::GameInstance::Get()->GetComponent<GameManager>();
}

void Obstacle::Start()
{
    // Get the player
    player = transform->GetTransformByTag("Player");
    previousPlayerPosition = player->GetWorldPosition();

    // Set random rotation
    const float randomAngle = (((rand() * rand())) % 1001) / 1000.0f * 2.0f * glm::pi<float>();
    transform->Rotate(glm::vec3(0.0f, randomAngle, 0.0f));
}

void Obstacle::Update(const float deltaTime)
{
    if (gameManager->GetGameState() != GameManager::Playing) {
        return;
    }

    // Check for player collision
    if (glm::distance(transform->GetWorldPosition(), player->GetWorldPosition()) < (playerRadius + collisionRadius)) {
        if (isHazard) {
            // Hit the player
            player->GetComponent<PlayerController>()->GetHit();
        }
        else {
            // Collect the present
            gameManager->UpdateScore(1);
        }

        // Destroy the object
        gameManager->GetSceneReference()->DestroyObject(transform);
    }

    // Distance dependant
    t += (player->GetWorldPosition() - previousPlayerPosition).z;
    previousPlayerPosition = player->GetWorldPosition();

    if (t > despawnDistance) {
        gameManager->GetSceneReference()->DestroyObject(transform);
    }
}

void Obstacle::Reset()
{
    // Delete the object
    gameManager->GetSceneReference()->DestroyObject(transform);
}
