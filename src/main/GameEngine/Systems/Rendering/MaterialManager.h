#pragma once
#include <string>
#include <unordered_map>

#include "Material.h"

namespace rendering
{
    class MaterialManager
    {
    public:
        static void InitMaterials();
        static Material* GetMaterial(const std::string& materialName);

        static const std::string MAT_DEFAULT_LIT;
        static const std::string MAT_TEST_SHADER_GRAPH;
        static const std::string MAT_HELICOPTER;
        static const std::string MAT_HEIGHT_MAP;
        static const std::string MAT_SCROLLABLE_TERRAIN;
        static const std::string MAT_SIMPLE;
        static const std::string MAT_COLOR;
        static const std::string MAT_MINIMAP;
        static const std::string MAT_MINIMAP_HEIGHTMAP;
        static const std::string MAT_SKYBOX;
        static const std::string MAT_TREE;
        
    private:
        static std::unordered_map<std::string, Material*> materials;
    };
}
