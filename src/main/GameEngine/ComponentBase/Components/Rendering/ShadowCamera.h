#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "main/GameEngine/MathUtils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/MeshRenderer.h"

class Serializer;

namespace component
{
    class Camera;
    class GameManager;
}

namespace component
{
    class MeshRenderer;

    class ShadowCamera : public Camera
    {
    public:
        ShadowCamera(transform::Transform* transform, const glm::vec3& center, const glm::vec3& up,
                const glm::vec2 viewportBottomLeft, const glm::vec2 viewportWidthHeight, 
                std::vector<int> layers, const bool autoResize = false) : Camera(transform, center, up, viewportBottomLeft, viewportWidthHeight, layers, autoResize) { }

        ShadowCamera(transform::Transform* transform, const glm::vec3& center = glm::vec3(0),
            const glm::vec3& up = glm::vec3_up, std::vector<int> layers = { 0, 1 }) : ShadowCamera(transform,
                center, up, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), layers, true) { }

        ~ShadowCamera() { }

        virtual void UpdateFrustum() override;
        virtual glm::mat4 GetViewMatrix() override;
        
        void FitOrthographicProjectionToCameras(const std::vector<Camera*>& cameras, glm::vec3 lightDirection);

        DirectionalLight* linkedDirectionalLight = nullptr;

    protected:
        glm::mat4 viewMatrix = glm::mat4(1.0f);
    };
}
