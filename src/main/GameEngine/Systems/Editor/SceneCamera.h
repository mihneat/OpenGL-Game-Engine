#pragma once
#include "main/GameEngine/GameEngine.h"
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    class Camera;
    
    class SceneCamera : public Camera
    {
    public:
        SceneCamera(transform::Transform* transform) : Camera(transform, glm::vec3(0),
            glm::vec3_up, { MeshRenderer::Default, MeshRenderer::UI }) { }
        ~SceneCamera() { }

        void UpdateValues(glm::vec2 resolution);
        
        void InputUpdate(float deltaTime, int mods) override;
        void MouseMove(const int mouseX, const int mouseY,
            const int deltaX, const int deltaY) override;
        void MouseBtnPress(const int mouseX, const int mouseY,
            const int button, const int mods) override;
        void MouseBtnRelease(const int mouseX, const int mouseY,
            const int button, const int mods) override;
        void MouseScroll(const int mouseX, const int mouseY,
            const int offsetX, const int offsetY) override;
        void WindowResize(int width, int height) override { }
        
        void FocusTransform(transform::Transform* transform);

    private:
        float speed = 30.0f;
        float superSpeed = 60.0f;

        float rotationSpeed = 0.2f;

        bool grabbed = false;

        friend class m1::GameEngine;
    };
}
