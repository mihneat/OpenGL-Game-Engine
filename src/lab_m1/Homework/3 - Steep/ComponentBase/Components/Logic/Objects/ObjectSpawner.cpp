#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Objects/ObjectSpawner.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;
using namespace prefabManager;

void ObjectSpawner::Start()
{
    // Get the player
    player = transform->GetTransformByTag("Player");
    previousPlayerPosition = player->GetWorldPosition();
    startingPlayerPosition = previousPlayerPosition;
    
    // Define default directions
    defaultForward = glm::normalize(glm::vec3(
        glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6.0f, glm::vec3_right) * glm::vec4(glm::vec3_forward, 1)
    ));

    defaultRight = glm::vec3_right;
}

void ObjectSpawner::Update(const float deltaTime)
{
    if (GameManager::GetInstance()->GetGameState() != GameManager::Playing) {
        return;
    }

    // Time dependant
    // t += deltaTime;

    // Distance dependant
    t += (player->GetWorldPosition() - previousPlayerPosition).z / 45.0f;
    previousPlayerPosition = player->GetWorldPosition();

    if (t > spawnTime) {
        t = 0.0f;
        SpawnObject();
        ResetTimer();
    }
}

void ObjectSpawner::SpawnObject()
{
    // cout << "Spawned Object\n";

    // Spawn a random object
    const int objectCount = 4;
    const int randomObject = (rand() * rand()) % objectCount;

    Transform* spawnedObject = NULL;

    switch (randomObject) {
    case 0:
        spawnedObject = PrefabManager::CreateTree(transform);
        break;

    case 1:
        spawnedObject = PrefabManager::CreateRock(transform);
        break;

    case 2:
        spawnedObject = PrefabManager::CreateLightPole(transform);
        break;

    case 3:
        spawnedObject = PrefabManager::CreatePresent(transform);
        break;

    default:
        break;
    }

    // Choose random spread distance
    const float randomNormalizedValue = 1.0f * ((rand() * rand()) % 1001) / 1000.0f; // [0, 1]
    const float randomSpawnSpread = randomNormalizedValue * (2.0f * spawnSpread) - spawnSpread; // [-spawnSpread, spawnSpread] 

    spawnedObject->Translate(player->GetWorldPosition() + defaultForward * spawnDistance + defaultRight * randomSpawnSpread);
}

void ObjectSpawner::ResetTimer()
{
    // Reset timer
    t = 0.0f;

    // Compute random spawn time
    const float randomNormalizedValue = 1.0f * ((rand() * rand()) % 1001) / 1000.0f; // [0, 1]
    spawnTime = randomNormalizedValue * (spawnTimeInterval.y - spawnTimeInterval.x) + spawnTimeInterval.x; // [minValue, maxValue] 
}

void ObjectSpawner::Reset()
{
    t = 0.0f;
    spawnTime = 1.0f;
    previousPlayerPosition = startingPlayerPosition;
}
