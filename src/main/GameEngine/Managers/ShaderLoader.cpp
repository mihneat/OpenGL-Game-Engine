#include "main/GameEngine/Managers/ShaderLoader.h"

#include "core/managers/resource_path.h"
#include "main/GameEngine/Systems/FileSystem.h"
#include "main/GameEngine/Systems/Rendering/ShaderResourceManager.h"

using namespace std;
using namespace loaders;
using namespace rendering;

void ShaderLoader::InitShaders()
{
    LoadShaderFromType(Standard);
    LoadShaderFromType(Simple);
    LoadShaderFromType(Color);
    LoadShaderFromType(VertexNormal);
    LoadShaderFromType(VertexColor);
}

// TODO: Read shader from a file
void ShaderLoader::LoadShaderFromFile(std::string shaderPath)
{
    // TODO: Parse the shader file

    std::string shaderName = "";
    std::string vertexShaderPath = "";
    std::string geometryShaderPath = "";
    std::string fragmentShaderPath = "";

    // Check if the name exists
    if (ShaderResourceManager::GetShader(shaderName) != nullptr)
        return;

    // Create the shader base
    Shader *newShader = LoadShader(shaderName, vertexShaderPath, fragmentShaderPath);

    // Parse the shader params

    ShaderResourceManager::AddShader(shaderName, newShader);
}

void ShaderLoader::LoadShaderFromType(ShaderType type)
{
    const Shader* newShader = nullptr;
    
    switch (type)
    {
    case Standard:
        newShader = LoadStandardShader();
        break;

    case Simple:
        newShader = LoadShader("Simple", "MVP.Texture.VS.glsl", "Default.FS.glsl");
        break;

    case Color:
        newShader = LoadShader("Color", "MVP.Texture.VS.glsl", "Color.FS.glsl");
        break;

    case VertexNormal:
        newShader = LoadShader("VertexNormal", "MVP.Texture.VS.glsl", "Normals.FS.glsl");
        break;

    case VertexColor:
        newShader = LoadShader("VertexColor", "MVP.Texture.VS.glsl", "VertexColor.FS.glsl");
        break;

    default:
        break;
    }

    // The shader has already been loaded
    if (newShader == nullptr)
        return;
}

Shader* ShaderLoader::LoadStandardShader()
{
    // Check if the name exists
    if (ShaderResourceManager::GetShader("GameEngine") != nullptr)
        return nullptr;
    
    Shader *newShader = LoadShader("GameEngine",
            "my_shaders/main_shader/GameEngine.VS.glsl",
            "my_shaders/main_shader/GameEngine.FS.glsl");
    
    ShaderResourceManager::AddShader("GameEngine", newShader);
    newShader->shaderParams.ints["is_scrolling"] = 0;
    newShader->shaderParams.vec2s["scroll_amount"] = glm::vec2(0.0f, 0.0f);
    newShader->shaderParams.floats["time_of_day"] = 1.0f;

    return newShader;
}

Shader* ShaderLoader::LoadShader(
    const std::string& shaderName,
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath)
{
    // Check if the name exists
    if (ShaderResourceManager::GetShader(shaderName) != nullptr)
        return nullptr;
    
    // Set the source directory
    const string sourceTextureDir = PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::SHADERS);

    // Create a shader program for drawing face polygon with the color of the normal
    Shader* newShader = new Shader(shaderName);
    newShader->AddShader(PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::SHADERS, vertexShaderPath), GL_VERTEX_SHADER);
    newShader->AddShader(PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::SHADERS, fragmentShaderPath), GL_FRAGMENT_SHADER);
    newShader->CreateAndLink();
    
    ShaderResourceManager::AddShader(shaderName, newShader);

    return newShader;
}
