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

namespace prefabManager {
    class PrefabManager;
}

namespace m1
{
    class GameEngine : public gfxc::SimpleScene
    {
    public:
        GameEngine();
        ~GameEngine();

        // friend class loaders::ShaderLoader;
        // friend class rendering::RenderingSystem;

        void Init() override;

        // void LoadTexture(std::string texId, std::string texPath);
        // ShaderBase* LoadShader(std::string shaderName, std::string vertexShaderPath, std::string fragmentShaderPath);

        void SetUniforms();

        glm::vec2 GetResolution();
        void DestroyObject(transform::Transform* object);

        gfxc::TextRenderer* GetTextRenderer() { return textRenderer; }
        WindowObject* GetWindow() { return window; }

    private:
        std::unordered_set<transform::Transform*> markedForDestruction;

        void FrameStart() override;
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

        void CreateHierarchy();
        void DestroyMarkedObjects();

        void UpdateGameLogic(float deltaTimeSeconds);
        void RenderGameView();
        void RenderSceneView();

    public:

        static void ApplyToComponents(
            transform::Transform* currentTransform,
            std::function<void(component::Component*)> componentApplication,
            std::function<void(transform::Transform*)> transformApplicationApriori = [](transform::Transform* comp) {},
            std::function<void(transform::Transform*)> transformApplicationAposteriori = [](transform::Transform* comp) {}
        );

    protected:

        void StartComponents(transform::Transform *currentTransform);
        void UpdateComponents(transform::Transform* currentTransform, const float deltaTime);
        void LateUpdateComponents(transform::Transform* currentTransform, const float deltaTime);
        void DeleteComponents(transform::Transform* currentTransform);

        glm::vec4 clearColor;
        gfxc::TextRenderer* textRenderer;

        component::Camera* mainCam;
        std::vector<component::Camera*> secondaryCams;

        bool useSceneCamera;

        glm::mat4 projectionMatrix;

        std::unordered_map<std::string, Texture2D*> mapTextures;

        std::unordered_set<transform::Transform*> loadedHierarchies;

    private:
        rendering::RenderingSystem* renderingSystem;

    public:
        float timeOfDay;

    };
}   // namespace m1
