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
    class PlayerController : public Component, public IResetable
    {
        MARK_SERIALIZABLE
        
    public:
        PlayerController(transform::Transform* transform) : Component(transform), speed(0.0f), maxSpeed(100.0f), acceleration(20.0f),
            tilt(0.0f), tiltFactor(0.4f), playerBody(NULL), initialPosition(glm::vec3()), maxLives(3), lives(0), skinIndex(0)
        { 
            defaultForward = glm::normalize(glm::vec3(
                glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6.0f, glm::vec3_right) * glm::vec4(glm::vec3_forward, 1)
            ));

            defaultUp = glm::normalize(glm::vec3(
                glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6.0f, glm::vec3_right) * glm::vec4(glm::vec3_up, 1)
            ));

            forward = defaultForward;
        }

        ~PlayerController() { }

        void Start();
        void Update(const float deltaTime);

        void Reset();

        void GetHit();

        int GetLives() { return lives; }
        void SetMaxSpeed(float newMaxSpeed) { maxSpeed = newMaxSpeed; }

        void ChangeSkin();

        void KeyPress(const int key, const int mods);
        void MouseMove(const int mouseX, const int mouseY, const int deltaX, const int deltaY);

    protected:
        SERIALIZE_FIELD float acceleration;
        SERIALIZE_FIELD float speed;
        SERIALIZE_FIELD float maxSpeed;

        glm::vec3 defaultForward, defaultUp;
        glm::vec3 forward;

        SERIALIZE_FIELD transform::Transform* playerBody;

        SERIALIZE_FIELD float tilt;
        SERIALIZE_FIELD float tiltFactor;

        SERIALIZE_FIELD int lives;
        SERIALIZE_FIELD int maxLives;

        SERIALIZE_FIELD glm::vec3 initialPosition;

        int skinIndex;
        std::vector<int> skins;

        GameManager* gameManager = nullptr;
    };
}
