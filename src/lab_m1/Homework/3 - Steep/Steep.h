#pragma once

#include "components/simple_scene.h"
#include "components/text_renderer.h"
#include "lab_m1/Homework/3 - Steep/PrefabManager.h"
#include "lab_m1/Homework/3 - Steep/Managers/TextureLoader.h"
#include "lab_m1/Homework/3 - Steep/Managers/ShaderLoader.h"
#include "lab_m1/Homework/3 - Steep/Managers/LightManager.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Transform.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Camera.h"

#include <stack>

namespace prefabManager {
    class PrefabManager;
}

namespace m1
{
    class Steep : public gfxc::SimpleScene
    {
    public:
        Steep();
        ~Steep();

        void Init() override;

        void GenerateMesh(
            Mesh* mesh,
            std::vector<VertexFormat> vertices,
            std::vector<unsigned int> indices
        );

        void DrawMesh(
            std::string meshName,
            std::string shader,
            glm::mat4 modelMatrix,
            bool useViewMatrix = true,
            std::string texture = "",
            float texScale = 1.0f
        );

        void LoadMesh(std::string meshId, std::string meshName, std::string meshPath);
        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const bool renderUI, const std::string texture, const float texScale);

        void LoadTexture(std::string texId, std::string texPath);
        void LoadShader(std::string shaderName, std::string vertexShaderPath, std::string fragmentShaderPath);

        void SetUniforms();

        glm::vec2 GetResolution();
        void DestroyObject(transform::Transform* object);
        bool GetDebugMode();

        gfxc::TextRenderer* GetTextRenderer() { return textRenderer; }
        WindowObject* GetWindow() { return window; }
        component::Camera* GetCurrentCamera() { return currCam; }

    private:
        std::stack<transform::Transform*> markedForDestruction;

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

    protected:

        void ApplyToComponents(
            transform::Transform* currentTransform,
            std::function<void(component::Component*)> componentApplication,
            std::function<void(transform::Transform*)> transformApplicationApriori = [](transform::Transform* comp) {},
            std::function<void(transform::Transform*)> transformApplicationAposteriori = [](transform::Transform* comp) {}
        );

        void StartComponents(transform::Transform *currentTransform);
        void UpdateComponents(transform::Transform* currentTransform, const float deltaTime);
        void LateUpdateComponents(transform::Transform* currentTransform, const float deltaTime);
        void RerenderComponents(transform::Transform* currentTransform, const float deltaTime);
        void DeleteComponents(transform::Transform* currentTransform);

        transform::Transform *hierarchy;

        bool debugMode;

        glm::vec4 clearColor;
        gfxc::TextRenderer* textRenderer;

        component::Camera* mainCam;
        component::Camera* currCam;
        std::vector<component::Camera*> secondaryCams;

        bool useSceneCamera;

        glm::mat4 projectionMatrix;

        std::unordered_map<std::string, Texture2D*> mapTextures;

        // Steep Shader params
        // TODO: Create a better shading system :(

    public:
        glm::vec2 playerOffset;
        float scaleFactor;
        float timeOfDay;

    protected:

    };
}   // namespace m1
