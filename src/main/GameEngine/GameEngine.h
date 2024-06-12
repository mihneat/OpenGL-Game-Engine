#pragma once

#include "components/simple_scene.h"
#include "components/text_renderer.h"
#include "main/GameEngine/PrefabManager.h"
#include "main/GameEngine/Managers/TextureLoader.h"
#include "main/GameEngine/Managers/ShaderLoader.h"
#include "main/GameEngine/Managers/LightManager.h"
#include "main/GameEngine/ComponentBase/Transform.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"

#include "Systems/SceneManager.h"

#include <stack>

namespace component
{
    class SceneCamera;
    class SceneManager;
}

namespace prefabManager {
    class PrefabManager;
}

namespace transform
{
    class Transform;
}

namespace m1
{
    class GameEngine : public gfxc::SimpleScene
    {
    public:
        GameEngine();
        ~GameEngine();

        void Init() override;

        void HandleSceneLoaded(transform::Transform* root);

    private:
        void FrameStart() override;
        void PreUpdate() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void OnGameWindowResize(int width, int height) override;

        void CreateSceneCamera();
        void SaveSceneToFile();
        void CreateHierarchy();
        void DestroyMarkedObjects();

        void UpdateGameLogic(float deltaTimeSeconds);
        void RenderGameView();
        void RenderSceneView();
        
        void ReloadScene();

    public:

        static void ApplyToComponents(
            transform::Transform* currentTransform,
            std::function<void(component::Component*)> componentApplication,
            std::function<void(transform::Transform*)> transformApplicationApriori = [](transform::Transform* comp) {},
            std::function<void(transform::Transform*)> transformApplicationAposteriori = [](transform::Transform* comp) {}
        );

    protected:

        void AwakeComponents(transform::Transform *currentTransform);
        void StartComponents(transform::Transform *currentTransform);
        void UpdateComponents(transform::Transform* currentTransform, const float deltaTime);
        void LateUpdateComponents(transform::Transform* currentTransform, const float deltaTime);
        void DeleteComponents(transform::Transform* currentTransform);
        
        void UpdateTransforms(transform::Transform* currentTransform);

        void FindCameras();

        glm::vec4 clearColor;
        gfxc::TextRenderer* textRenderer;

        component::Camera* mainCam = nullptr;
        std::vector<component::Camera*> secondaryCams;

        std::string startScene;

    private:
        rendering::RenderingSystem* renderingSystem;
        component::SceneCamera* sceneCamera;

    };
}   // namespace m1
