#include "MaterialManager.h"

#include "ShaderResourceManager.h"
#include "main/GameEngine/Serialization/Database.h"

using namespace rendering;

const std::string MaterialManager::MAT_DEFAULT_LIT = "DefaultLit";
const std::string MaterialManager::MAT_TEST_SHADER_GRAPH = "TestShaderGraph";
const std::string MaterialManager::MAT_HELICOPTER = "Helicopter";
const std::string MaterialManager::MAT_SCROLLABLE_TERRAIN = "ScrollableTerrain";
const std::string MaterialManager::MAT_HEIGHT_MAP = "HeightMap";
const std::string MaterialManager::MAT_SIMPLE = "Simple";
const std::string MaterialManager::MAT_COLOR = "Color";
const std::string MaterialManager::MAT_SKYBOX = "Skybox";
const std::string MaterialManager::MAT_MINIMAP = "Minimap";
const std::string MaterialManager::MAT_MINIMAP_HEIGHTMAP = "MinimapHeightMap";
const std::string MaterialManager::MAT_TREE = "Tree";

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
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_HEIGHT_MAP));
        
        materials[MAT_HEIGHT_MAP] = material;

        Database::AddEntry({"100002", "Height Map", "Material", material});
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
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_STANDARD));
        material->params.ints["is_helicopter"] = 1;
        material->params.floats["bend_factor"] = 0.0f;
        
        materials[MAT_HELICOPTER] = material;

        Database::AddEntry({"100005", "Helicopter", "Material", material});
    }
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_SKYBOX));

        materials[MAT_SKYBOX] = material;

        Database::AddEntry({"100006", "Skybox", "Material", material});
    }
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_TREE));
        material->params.ints["ignore_water"] = 1;
        
        materials[MAT_TREE] = material;

        Database::AddEntry({"100007", "Tree", "Material", material});
    }
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_MINIMAP));

        materials[MAT_MINIMAP] = material;

        Database::AddEntry({"100008", "Minimap", "Material", material});
    }
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_MINIMAP));
        material->params.ints["draw_heightmap"] = 1;

        materials[MAT_MINIMAP_HEIGHTMAP] = material;

        Database::AddEntry({"100009", "Minimap Height Map", "Material", material});
    }
    
    {
        Material* material = new Material(ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_TEST_SHADER_GRAPH));

        materials[MAT_TEST_SHADER_GRAPH] = material;

        Database::AddEntry({"100010", "Test Shader Graph", "Material", material});
    }
}

Material* MaterialManager::GetMaterial(const std::string& materialName)
{
    if (materials.find(materialName) == materials.end())
        return nullptr;

    return materials[materialName];
}
