#pragma once
#include <string>
#include <unordered_map>

#include "core/gpu/mesh.h"

namespace rendering
{
    class MeshResourceManager
    {
    public:
        static void LoadMesh(std::string meshId, std::string meshName, std::string meshPath);

        static void GenerateMesh(
            const std::string& meshName,
            int drawMode,
            const std::vector<VertexFormat>& vertices,
            const std::vector<unsigned int>& indices
        );

    public:
        static std::unordered_map<std::string, Mesh *> meshes;
    
    };
}
