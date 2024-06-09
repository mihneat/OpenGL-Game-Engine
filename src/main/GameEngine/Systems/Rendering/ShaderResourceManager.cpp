#include "ShaderResourceManager.h"

#include "main/GameEngine/Systems/FileSystem.h"

using namespace rendering;

const std::string ShaderResourceManager::SHADER_STANDARD = "GameEngine";
const std::string ShaderResourceManager::SHADER_SIMPLE = "Simple";
const std::string ShaderResourceManager::SHADER_COLOR = "Color";

std::unordered_map<std::string, Shader*> ShaderResourceManager::shaders;

Shader* ShaderResourceManager::GetShader(const std::string& shaderName)
{
    if (shaders.find(shaderName) == shaders.end())
        return nullptr;

    return shaders[shaderName];
}
    
void ShaderResourceManager::AddShader(const std::string& shaderName, Shader* shader)
{
    shaders[shaderName] = shader;
}