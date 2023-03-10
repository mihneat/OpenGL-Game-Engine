#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/MeshRenderer.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

namespace managers
{
    class GameManager;
}

namespace component
{
    class MeshRenderer;
    enum class LayerEnum;

    class Camera : public Component
    {
    public:
        Camera(transform::Transform* transform, const glm::vec3& center, const glm::vec3& up,
                const glm::vec2 viewportBottomLeft, const glm::vec2 viewportWidthHeight, 
                std::vector<int> layers, const bool autoResize = false) : Component(transform),
            viewportBottomLeft(viewportBottomLeft), viewportWidthHeight(viewportWidthHeight), autoResize(autoResize)
        {
            Set(transform->GetLocalPosition(), center, up);
            distanceToTarget = glm::distance(center, transform->GetLocalPosition());
            SetProjection(60, 16.0f / 9.0f);

            for (int i = 0; i < (int)layers.size(); ++i) {
                this->layers.insert(layers[i]);
            }
        }

        Camera(transform::Transform* transform, const glm::vec3& center,
            const glm::vec3& up, std::vector<int> layers) : Camera(transform,
                center, up, glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), layers, true) { }

        ~Camera() { }

        void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);

        void SetProjection(const float fov, const float aspectRatio);
        void SetOrtographic(const float width, const float height);

        void MoveForward(float distance);

        void TranslateForward(float distance);
        void TranslateUpward(float distance);
        void TranslateRight(float distance);

        void RotateFirstPerson_OX(float angle);
        void RotateFirstPerson_OY(float angle);
        void RotateFirstPerson_OZ(float angle);

        void RotateThirdPerson_OX(float angle);
        void RotateThirdPerson_OY(float angle);
        void RotateThirdPerson_OZ(float angle);

        void WindowResize(int width, int height);

        // C++ headers are too fkin stupid and hard
        // TODO: Replace int with MeshRenderer::LayerEnum
        bool IsLayerRendered(int layer);

        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix() { return projectionMatrix; }
        glm::vec3 GetTargetPosition();
        glm::vec4 GetViewportDimensions() { return glm::vec4(viewportBottomLeft.x, viewportBottomLeft.y,
            viewportWidthHeight.x, viewportWidthHeight.y); }

        glm::vec2 viewportBottomLeft;
        glm::vec2 viewportWidthHeight;

    protected:
        float distanceToTarget;
        glm::mat4 projectionMatrix;
        bool autoResize;

        std::unordered_set<int> layers;
    };
}
