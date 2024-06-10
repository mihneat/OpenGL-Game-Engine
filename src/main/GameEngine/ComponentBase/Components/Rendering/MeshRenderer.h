#pragma once

#include <unordered_set>

#include "main/GameEngine/GameEngine.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Interfaces/IRenderable.h"
#include "main/GameEngine/Systems/Rendering/MaterialOverrides.h"
#include "main/GameEngine/Systems/Rendering/Material.h"
#include "main/GameEngine/Systems/Rendering/RenderingSystem.h"

namespace prefabManager
{
    class PrefabManager;
}

namespace m1
{
    class GameEngine;
}

namespace component
{
    struct mesh_desc {
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;
        int drawMode;
    };
    
    SERIALIZE_CLASS
    class MeshRenderer : public Component, public IRenderable
    {
        MARK_SERIALIZABLE

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
            MeshEnum meshType = Cube,
            std::string meshName = "New Mesh",
            const rendering::Material* material = nullptr,
            LayerEnum layer = Default,
            glm::vec3 meshScale = glm::vec3(1.0f),
            glm::vec4 meshColor = glm::vec4(1.0f),
            bool renderUI = true,
            bool debugOnly = false
        );
        ~MeshRenderer();

        friend class rendering::RenderingSystem;

        void Init();

        void SetColor(glm::vec4 newColor);
        void SetTexture(std::string newTexture, glm::vec2 newTexScale = glm::vec2(1.0f));
        void SetMaterialOverrides(rendering::MaterialOverrides* materialOverrides);

        const rendering::Material* GetMaterial() const { return material; }
        rendering::MaterialOverrides* GetMaterialOverrides() const { return materialOverrides; }

        LayerEnum GetLayer() { return layer; }

        // To be implemented when needed
        // void ChangeMesh(std::string newMeshName);

        static std::unordered_set<std::string> meshNames;

    protected:
        MeshEnum type = Cube;
        std::string baseMeshName = "New Mesh";
        std::string meshName;
        SERIALIZE_FIELD glm::vec4 color = glm::vec4(1);
        SERIALIZE_FIELD glm::vec3 meshScale = glm::vec3(1);

        mesh_desc CreateSquare();
        mesh_desc CreateFragmentedSquare();
        mesh_desc CreateCircle(const int circleVertexCount, const bool makeRainbow, const bool makeHollow);
        mesh_desc CreateCube();
        mesh_desc CreateRoad();

    private:
        SERIALIZE_FIELD bool debugOnly = false;
        bool generateMesh = true;
        bool renderInWorldSpace = true;
        LayerEnum layer = Default;
        std::string texture = "";
        SERIALIZE_FIELD glm::vec2 texScale = glm::vec2(1, 1);

        const rendering::Material* material = nullptr;
        rendering::MaterialOverrides* materialOverrides = nullptr;

        bool initialized = false;

        void MeshFactory();
        void LoadMesh(const std::string name, const std::string path);

        friend class rendering::RenderingSystem;
        
    };
}   // namespace component
