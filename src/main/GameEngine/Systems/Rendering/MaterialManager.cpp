#include "MaterialManager.h"

#include "ShaderResourceManager.h"

using namespace rendering;

const std::string MaterialManager::MAT_DEFAULT_LIT = "DefaultLit";
const std::string MaterialManager::MAT_SCROLLABLE_TERRAIN = "ScrollableTerrain";
const std::string MaterialManager::MAT_SIMPLE = "Simple";
const std::string MaterialManager::MAT_COLOR = "Color";

std::unordered_map<std::string, Material*> MaterialManager::materials;

void MaterialManager::InitMaterials()
{
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_STANDARD));

        materials[MAT_DEFAULT_LIT] = material;
    }

    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_STANDARD));
        material->params.ints["is_scrolling"] = 1;
        
        materials[MAT_SCROLLABLE_TERRAIN] = material;
    }
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_SIMPLE));

        materials[MAT_SIMPLE] = material;
    }
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_COLOR));

        materials[MAT_COLOR] = material;
    }
}

Material* MaterialManager::GetMaterial(const std::string& materialName)
{
    if (materials.find(materialName) == materials.end())
        return nullptr;

    return materials[materialName];
}
