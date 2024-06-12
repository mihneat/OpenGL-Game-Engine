#include "MaterialManager.h"

#include "ShaderResourceManager.h"
#include "main/GameEngine/Serialization/Database.h"

using namespace rendering;

const std::string MaterialManager::MAT_DEFAULT_LIT = "DefaultLit";
const std::string MaterialManager::MAT_SCROLLABLE_TERRAIN = "ScrollableTerrain";
const std::string MaterialManager::MAT_SIMPLE = "Simple";
const std::string MaterialManager::MAT_COLOR = "Color";

std::unordered_map<std::string, Material*> MaterialManager::materials;

void MaterialManager::InitMaterials()
{
    // TODO(future): Load materials from files
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_STANDARD));

        materials[MAT_DEFAULT_LIT] = material;

        Database::AddEntry({"100001", "Default Lit", "Material", material});
    }

    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_STANDARD));
        material->params.ints["is_scrolling"] = 1;
        
        materials[MAT_SCROLLABLE_TERRAIN] = material;

        Database::AddEntry({"100002", "Scrollable Terrain", "Material", material});
    }
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_SIMPLE));

        materials[MAT_SIMPLE] = material;

        Database::AddEntry({"100003", "Simple", "Material", material});
    }
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_COLOR));

        materials[MAT_COLOR] = material;

        Database::AddEntry({"100004", "Color", "Material", material});
    }
}

Material* MaterialManager::GetMaterial(const std::string& materialName)
{
    if (materials.find(materialName) == materials.end())
        return nullptr;

    return materials[materialName];
}
