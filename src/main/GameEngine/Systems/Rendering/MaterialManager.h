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
        static const std::string MAT_SCROLLABLE_TERRAIN;
        static const std::string MAT_SIMPLE;
        static const std::string MAT_COLOR;
        
    private:
        static std::unordered_map<std::string, Material*> materials;
    };
}
