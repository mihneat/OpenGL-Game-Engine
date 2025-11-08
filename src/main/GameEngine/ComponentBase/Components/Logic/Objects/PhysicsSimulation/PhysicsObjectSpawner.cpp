#include "PhysicsObjectSpawner.h"

#include "main/GameEngine/PrefabManager.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Physics/Rigidbody.h"

using namespace std;
using namespace component;
using namespace component;
using namespace transform;
using namespace prefabManager;

void PhysicsObjectSpawner::Start()
{
    spawnParent = transform->GetTransformByTag("SpawnParent");
}

void PhysicsObjectSpawner::SpawnObjects()
{
    spawnOrder.clear();
    if (spawnParent == nullptr)
        return;

    for (int i = 0; i < spheresToSpawn; ++i)
        spawnOrder.push_back(0);

    for (int i = 0; i < cubesToSpawn; ++i)
        spawnOrder.push_back(1);

    // for (int i = 0; i < conesToSpawn; ++i)
    //     spawnOrder.push_back(2);

    // Here is the place to shuffle the array if necessary

    for (int i : spawnOrder)
        SpawnObject(i);
}

void PhysicsObjectSpawner::SpawnObject(int objType)
{
    Transform* spawnedObject = nullptr;

    switch (objType) {
    case 0:
        spawnedObject = Transform::Instantiate(PrefabManager::CreatePhysicsSphere(spawnParent));
        break;

    case 1:
        spawnedObject = Transform::Instantiate(PrefabManager::CreatePhysicsCube(spawnParent));
        break;

    case 2:
        spawnedObject = Transform::Instantiate(PrefabManager::CreatePhysicsCone(spawnParent));
        break;

    default:
        break;
    }

    // Choose a random place to spawn it
    const glm::vec3 randomNormalizedValues(
        1.0f * (rand() * rand() % 1001) / 1000.0f, // [0, 1]
        1.0f * (rand() * rand() % 1001) / 1000.0f, // [0, 1]
        1.0f * (rand() * rand() % 1001) / 1000.0f  // [0, 1]
    );

    glm::vec3 randomOffset = randomNormalizedValues * (2.0f * spawnBoxSize) - spawnBoxSize;
    spawnedObject->Translate(transform->GetWorldPosition() + randomOffset);
}

void PhysicsObjectSpawner::Reset()
{
    spawnOrder.clear();
    
    if (spawnParent == nullptr)
        return;
    
    // Destroy all the spawned objects
    for (int i = spawnParent->GetChildCount() - 1; i >= 0; i--)
        spawnParent->RemoveChild(spawnParent->GetChild(i));
}

void PhysicsObjectSpawner::KeyPress(const int key, const int mods)
{
    if (key == GLFW_KEY_R)
    {
        Reset();
    } else if (key == GLFW_KEY_0)
    {
        spheresToSpawn = spheresCustomBatch;
        cubesToSpawn = cubesCustomBatch;
        conesToSpawn = conesCustomBatch;

        Reset();
        SpawnObjects();
    } else if (key == GLFW_KEY_1)
    {
        spheresToSpawn = spheresBatch1;
        cubesToSpawn = cubesBatch1;
        conesToSpawn = conesBatch1;

        Reset();
        SpawnObjects();
    } else if (key == GLFW_KEY_2)
    {
        spheresToSpawn = spheresBatch2;
        cubesToSpawn = cubesBatch2;
        conesToSpawn = conesBatch2;

        Reset();
        SpawnObjects();
    } else if (key == GLFW_KEY_3)
    {
        spheresToSpawn = spheresBatch3;
        cubesToSpawn = cubesBatch3;
        conesToSpawn = conesBatch3;

        Reset();
        SpawnObjects();
    } else if (key == GLFW_KEY_SPACE)
    {
        for (int i = spawnParent->GetChildCount() - 1; i >= 0; i--) {
            Rigidbody* rb = spawnParent->GetChild(i)->GetComponent<Rigidbody>();
            if (rb == nullptr)
                continue;
            
            const glm::vec3 randomVector(
                1.0f * (rand() * rand() % 1001) / 500.0f - 1.0f, // [-1, 1]
                1.0f * (rand() * rand() % 1001) / 500.0f - 1.0f, // [-1, 1]
                1.0f * (rand() * rand() % 1001) / 500.0f - 1.0f  // [-1, 1]
            );
            
            const float randomForceMagnitude = 1.0f * (rand() * rand() % 200);

            rb->AddForce(randomVector * randomForceMagnitude);
        }
    }
}
