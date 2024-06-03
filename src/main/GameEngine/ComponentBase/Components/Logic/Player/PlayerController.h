#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

namespace component
{
    class PlayerController : public Component, public IResetable
    {
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

        int GetLives() { return lives; };
        void SetMaxSpeed(float newMaxSpeed) { maxSpeed = newMaxSpeed; }

        void ChangeSkin();

        void KeyPress(const int key, const int mods);
        void MouseMove(const int mouseX, const int mouseY, const int deltaX, const int deltaY);

    protected:
        float acceleration;
        float speed, maxSpeed;

        glm::vec3 defaultForward, defaultUp;
        glm::vec3 forward;

        transform::Transform* playerBody;

        float tilt;
        float tiltFactor;

        int lives;
        const int maxLives;

        glm::vec3 initialPosition;

        int skinIndex;
        std::vector<int> skins;
    };
}
