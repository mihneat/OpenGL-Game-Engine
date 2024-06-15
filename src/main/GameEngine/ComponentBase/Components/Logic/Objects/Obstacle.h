#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

namespace component
{
    SERIALIZE_CLASS
    class Obstacle : public Component, public IResetable
    {
        MARK_SERIALIZABLE(Obstacle)
        
    public:
        Obstacle(transform::Transform* transform, float collisionRadius = 3.0f, bool isHazard = true) : Component(transform),
            collisionRadius(collisionRadius), isHazard(isHazard) { }
        ~Obstacle() { }

        void Awake();
        void Start();
        void Update(const float deltaTime);

        void Reset();

    protected:
        component::GameManager* gameManager;

        float playerRadius = 4.0f;

        SERIALIZE_FIELD float collisionRadius = 3.0f;
        SERIALIZE_FIELD bool isHazard = true;

        transform::Transform* player = nullptr;
        glm::vec3 previousPlayerPosition = glm::vec3();

        float t = 0.0f, despawnDistance = 220.0f;
    };
}
