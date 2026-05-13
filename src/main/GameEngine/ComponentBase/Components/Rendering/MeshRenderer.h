#pragma once

#include <unordered_map>

#include "main/GameEngine/GameEngine.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Interfaces/IRenderable.h"
#include "main/GameEngine/Systems/Rendering/MaterialOverrides.h"
#include "main/GameEngine/Systems/Rendering/Material.h"
#include "main/GameEngine/Systems/Rendering/RenderingSystem.h"
#include "main/GameEngine/Systems/Rendering/Texture.h"

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
    struct ExtraMeshData
    {
        utils::AABB boundingBox;

        ExtraMeshData() : boundingBox({}, {}) { }
    };
    
    struct mesh_desc {
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;
        int drawMode = GL_TRIANGLES;
    };
    
    SERIALIZE_CLASS
    class MeshRenderer : public Component, public IRenderable
    {
        MARK_SERIALIZABLE(MeshRenderer)

    public:
        SERIALIZE_ENUM
        enum MeshEnum {
            Square,
            FragmentedSquare,
            Circle,
            Cylinder,
            Cube,
            CubeMesh,
            Sphere,
            Heart,
            Cone,
            Line,
            None
        };

        SERIALIZE_ENUM
        enum LayerEnum {
            Default,
            UI,
            Minimap
        };

        SERIALIZE_ENUM
        enum FaceCullingMode {
            CullNone,
            CullBack,
            CullFront,
            CullBoth
        };

        MeshRenderer(
            transform::Transform* transform,
            MeshEnum meshType = Cube,
            std::string meshName = "New Mesh",
            rendering::Material* material = nullptr,
            LayerEnum layer = Default,
            glm::vec3 meshScale = glm::vec3(1.0f),
            glm::vec4 meshColor = glm::vec4(1.0f),
            bool renderInWorldSpace = true,
            bool debugOnly = false
        );
        ~MeshRenderer();

        friend class rendering::RenderingSystem;

        void SetMesh(MeshEnum newMeshType);
        void SetColor(glm::vec4 newColor);
        void SetTexture(rendering::Texture* newTexture);
        void SetTexture2(rendering::Texture* newTexture);
        void SetTexture3(rendering::Texture* newTexture);
        void SetTexture4(rendering::Texture* newTexture);
        void SetTextureScale(glm::vec2 newTexScale);
        void SetMaterialOverrides(rendering::MaterialOverrides* materialOverrides);
        void SetFaceCullingMode(FaceCullingMode newFaceCullingMode);

        const rendering::Material* GetMaterial() const { return material; }
        rendering::MaterialOverrides* GetMaterialOverrides() const { return materialOverrides; }

        LayerEnum GetLayer() { return layer; }

        bool IsInFrustum(const utils::Frustum& frustum) const;

        // To be implemented when needed
        // void ChangeMesh(std::string newMeshName);

        static std::unordered_map<MeshEnum, ExtraMeshData> loadedMeshes;

    protected:
        SERIALIZE_FIELD MeshEnum meshType = Cube;
        SERIALIZE_FIELD glm::vec4 color = glm::vec4(1);
        SERIALIZE_FIELD glm::vec3 meshScale = glm::vec3(1); // This needs to be deprecated
        SERIALIZE_FIELD FaceCullingMode faceCullingMode = CullBack;

        void GenerateTangentVectors(std::vector<VertexFormat>& vertices, glm::ivec3 indices);

        mesh_desc CreateSquare();
        mesh_desc CreateFragmentedSquare();
        mesh_desc CreateCircle(const int circleVertexCount, const bool makeRainbow, const bool makeHollow);
        mesh_desc CreateCylinder(int segmentCount);
        mesh_desc CreateCube();
        mesh_desc CreateLine();
        mesh_desc CreateRoad();

    private:
        SERIALIZE_FIELD bool debugOnly = false;
        bool generateMesh = true;
        SERIALIZE_FIELD bool renderInWorldSpace = true;
        SERIALIZE_FIELD bool useNormalMaps = false;
        SERIALIZE_FIELD LayerEnum layer = Default;
        SERIALIZE_FIELD rendering::Texture* texture1 = nullptr;
        SERIALIZE_FIELD rendering::Texture* texture2 = nullptr;
        SERIALIZE_FIELD rendering::Texture* texture3 = nullptr;
        SERIALIZE_FIELD rendering::Texture* texture4 = nullptr;
        SERIALIZE_FIELD rendering::Texture* normal1 = nullptr;
        SERIALIZE_FIELD rendering::Texture* normal2 = nullptr;
        SERIALIZE_FIELD rendering::Texture* normal3 = nullptr;
        SERIALIZE_FIELD rendering::Texture* normal4 = nullptr;
        SERIALIZE_FIELD glm::vec2 texScale = glm::vec2(1, 1);

        SERIALIZE_FIELD rendering::Material* material = nullptr;
        rendering::MaterialOverrides* materialOverrides = nullptr;

        bool initialized = false;

        void MeshFactory();
        void LoadMesh(const std::string name, const std::string path);

        void GenerateAABB();

        friend class rendering::RenderingSystem;
        friend class m1::GameEngine;
        
    };
}   // namespace component
