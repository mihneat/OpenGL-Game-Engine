#pragma once
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/Systems/Rendering/Material.h"

namespace component
{
    SERIALIZE_CLASS
    class HelicopterMovement : public Component
    {
        MARK_SERIALIZABLE(HelicopterMovement)

        HelicopterMovement(transform::Transform* transform) : Component(transform)
        {
        }

    public:
        void Start() override;
        void Update(float deltaTime) override;
        
        void MouseBtnPress(int mouseX, int mouseY, int button, int mods) override;

    private:
        glm::vec3 destination = glm::vec3(0.0f);
        
        SERIALIZE_FIELD rendering::Material* defaultLitMaterial = nullptr;
        SERIALIZE_FIELD rendering::Material* testShaderGraphMaterial = nullptr;
        SERIALIZE_FIELD rendering::Material* heightMapMaterial = nullptr;
        SERIALIZE_FIELD rendering::Material* helicopterMaterial = nullptr;
        SERIALIZE_FIELD rendering::Material* skyboxMaterial = nullptr;
        SERIALIZE_FIELD rendering::Material* treeMaterial = nullptr;

        SERIALIZE_FIELD float speed = 3.0f;
        SERIALIZE_FIELD float turnSpeed = 0.5f;
        SERIALIZE_FIELD float tiltAngle = -20.0f;

        transform::Transform* instancesParent = nullptr;
        
        bool hasReachedDestination = true;
        transform::Transform* spawnedMarker = nullptr;
        
        transform::Transform* modelTransform = nullptr;

        float targetAngle = 0.0f;
        float targetTiltAngle = 0.0f;
    };
}
