#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

namespace component
{
    class Obstacle : public Component, public IResetable
    {
        SERIALIZE_CLASS;
        
    public:
        Obstacle(transform::Transform* transform, float collisionRadius, bool isHazard = true) : Component(transform), player(NULL), t(0.0f), despawnDistance(220.0f),
            previousPlayerPosition(glm::vec3()), playerRadius(4.0f), collisionRadius(collisionRadius), isHazard(isHazard) { }
        ~Obstacle() { }

        void Start();
        void Update(const float deltaTime);

        void Reset();

    protected:
        managers::GameManager* gameManager;

        const float playerRadius;

        SERIALIZE_FIELD float collisionRadius;
        SERIALIZE_FIELD bool isHazard;

        transform::Transform* player;
        glm::vec3 previousPlayerPosition;

        float t, despawnDistance;
    };
}
