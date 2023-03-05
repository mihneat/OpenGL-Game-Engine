#pragma once

#include <unordered_set>

#include "lab_m1/Homework/3 - Steep/Steep.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Interfaces/IRenderable.h"

namespace prefabManager
{
    class PrefabManager;
}

namespace m1
{
    class Steep;
}

namespace component
{
    struct mesh_desc {
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;
        int drawMode;
    };

    class MeshRenderer : public Component, public IRenderable
    {

    public:
        enum MeshEnum {
            Square,
            FragmentedSquare,
            Circle,
            Cube,
            CubeMesh,
            Sphere,
            ZeldaHeart,
            Cone,
            None
        };

        enum LayerEnum {
            Default,
            UI
        };

        MeshRenderer(
            transform::Transform* transform,
            MeshEnum meshType,
            std::string meshName,
            std::string shader,
            LayerEnum layer = Default,
            glm::vec3 meshScale = glm::vec3(1.0f),
            glm::vec4 meshColor = glm::vec4(1.0f),
            bool renderUI = true,
            bool debugOnly = false
        );
        ~MeshRenderer();

        void Start();
        void LateUpdate(const float deltaTime);

        void SetScale(glm::vec3 newScale);
        void SetColor(glm::vec4 newColor);
        void SetTexture(std::string newTexture, float newTexScale = 1.0f);

        LayerEnum GetLayer() { return layer; }

        // To be implemented when needed
        // void ChangeMesh(std::string newMeshName);

        static std::unordered_set<std::string> meshNames;

    protected:
        m1::Steep* scene;
        MeshEnum type;
        std::string baseMeshName;
        std::string meshName;
        glm::vec4 color;
        glm::mat4 scaleMatrix;

        mesh_desc CreateSquare();
        mesh_desc CreateFragmentedSquare();
        mesh_desc CreateCircle(const int circleVertexCount, const bool makeRainbow, const bool makeHollow);
        mesh_desc CreateCube();
        mesh_desc CreateRoad();

    private:
        bool debugOnly;
        long long meshIndex;
        bool generateMesh;
        bool renderUI;
        std::string shader;
        LayerEnum layer;
        std::string texture;
        float texScale;

        void MeshFactory();
        void LoadMesh(const std::string name, const std::string path);

    };
}   // namespace component
