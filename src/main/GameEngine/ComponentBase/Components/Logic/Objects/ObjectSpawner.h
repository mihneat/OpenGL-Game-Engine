#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

namespace component
{
    class ObjectSpawner : public Component, public IResetable
    {
    public:
        ObjectSpawner(transform::Transform* transform) : Component(transform), t(0.0f), spawnTime(1.0f), spawnTimeInterval(glm::vec2(0.1f, 0.2f)),
            spawnDistance(175.0f), spawnSpread(250.0f), defaultForward(glm::vec3()), defaultRight(glm::vec3()), startingPlayerPosition(glm::vec3()),
            player(NULL), previousPlayerPosition(glm::vec3(0.0f)) { }
        ~ObjectSpawner() { }

        void Start();
        void Update(const float deltaTime);

        void SpawnObject();
        void ResetTimer();

        void Reset();

    protected:
        transform::Transform* player;
        glm::vec3 previousPlayerPosition, startingPlayerPosition;

        float t, spawnTime;
        glm::vec2 spawnTimeInterval;

        float spawnDistance, spawnSpread;
        glm::vec3 defaultForward, defaultRight;
    };
}
