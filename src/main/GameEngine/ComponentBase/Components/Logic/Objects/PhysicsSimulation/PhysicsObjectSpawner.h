#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

namespace component
{
    SERIALIZE_CLASS
    class PhysicsObjectSpawner : public Component
    {
        MARK_SERIALIZABLE(PhysicsObjectSpawner)
        
    public:
        PhysicsObjectSpawner(transform::Transform* transform) : Component(transform) { }
        ~PhysicsObjectSpawner() { }

        void Start() override;
        
        void KeyPress(const int key, const int mods) override;

        void SpawnObjects();
        void SpawnObject(int objType);

        void Reset();

    protected:
        SERIALIZE_FIELD glm::vec3 spawnBoxSize = glm::vec3(1.0f);
        SERIALIZE_FIELD int spheresCustomBatch = 10;
        SERIALIZE_FIELD int cubesCustomBatch = 25;
        SERIALIZE_FIELD int conesCustomBatch = 50;
        
        int spheresBatch1 = 100;
        int cubesBatch1 = 250;
        int conesBatch1 = 500;
        
        int spheresBatch2 = 250;
        int cubesBatch2 = 500;
        int conesBatch2 = 1000;
        
        int spheresBatch3 = 500;
        int cubesBatch3 = 1000;
        int conesBatch3 = 2500;

        int spheresToSpawn = 0;
        int cubesToSpawn = 0;
        int conesToSpawn = 0;

        transform::Transform* spawnParent = nullptr;

        // This is used in case to shuffle the vector at some point
        std::vector<int> spawnOrder;
    };
}
