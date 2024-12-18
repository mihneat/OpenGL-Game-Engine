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
    LoadShaderFromType(Tree);
    LoadShaderFromType(HeightMap);
    LoadShaderFromType(Skybox);
    LoadShaderFromType(Minimap);
    LoadShaderFromType(ViewColorTexture);
    LoadShaderFromType(ViewDepthTexture);
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
        
    case Tree:
        newShader = LoadTreeShader();
        break;
        
    case HeightMap:
        newShader = LoadHeightMapShader();
        break;
        
    case Skybox:
        newShader = LoadSkyboxShader();
        break;
        
    case Minimap:
        newShader = LoadMinimapShader();
        break;
        
    case ViewColorTexture:
        newShader = LoadViewColorTextureShader();
        break;
        
    case ViewDepthTexture:
        newShader = LoadViewDepthTextureShader();
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
            "Shaders/GameEngine.VS.glsl",
            "Shaders/GameEngine.FS.glsl",
            true);
    
    ShaderResourceManager::AddShader("GameEngine", newShader);
    newShader->shaderParams.ints["use_texture"] = 1;
    newShader->shaderParams.ints["is_helicopter"] = 0;
    newShader->shaderParams.ints["ignore_water"] = 0;
    newShader->shaderParams.floats["time_of_day"] = 1.0f;
    newShader->shaderParams.floats["bend_factor"] = 0.003f;
    newShader->shaderParams.vec3s["helicopter_position"] = glm::vec3(0);

    return newShader;
}

Shader* ShaderLoader::LoadHeightMapShader()
{
    // Check if the name exists
    if (ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_HEIGHT_MAP) != nullptr)
        return nullptr;
    
    Shader *newShader = LoadShader(ShaderResourceManager::SHADER_HEIGHT_MAP,
            "Shaders/HeightMap/HeightMap.VS.glsl",
            "Shaders/HeightMap/HeightMap.FS.glsl",
            true);
    
    ShaderResourceManager::AddShader(ShaderResourceManager::SHADER_HEIGHT_MAP, newShader);
    newShader->shaderParams.ints["use_texture"] = 1;
    newShader->shaderParams.floats["time_of_day"] = 1.0f;
    newShader->shaderParams.floats["bend_factor"] = 0.003f;
    newShader->shaderParams.vec3s["helicopter_position"] = glm::vec3(0);

    return newShader;
}

Shader* ShaderLoader::LoadSkyboxShader()
{
    // Check if the name exists
    if (ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_SKYBOX) != nullptr)
        return nullptr;
    
    Shader *newShader = LoadShader(ShaderResourceManager::SHADER_SKYBOX,
            "Shaders/Skybox/Skybox.VS.glsl",
            "Shaders/Skybox/Skybox.FS.glsl",
            true);
    
    ShaderResourceManager::AddShader(ShaderResourceManager::SHADER_SKYBOX, newShader);
    newShader->shaderParams.vec3s["helicopter_position"] = glm::vec3(0);

    return newShader;
}

Shader* ShaderLoader::LoadTreeShader()
{
    // Check if the name exists
    if (ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_TREE) != nullptr)
        return nullptr;
    
    Shader *newShader = LoadShader(ShaderResourceManager::SHADER_TREE,
            "Shaders/GameEngine.VS.glsl",
            "Shaders/Tree/Tree.FS.glsl",
            true);
    
    ShaderResourceManager::AddShader(ShaderResourceManager::SHADER_TREE, newShader);
    newShader->shaderParams.ints["use_texture"] = 1;
    newShader->shaderParams.ints["is_helicopter"] = 0;
    newShader->shaderParams.ints["ignore_water"] = 0;
    newShader->shaderParams.ints["distance_from_leaf"] = 2;
    newShader->shaderParams.floats["time_of_day"] = 1.0f;
    newShader->shaderParams.floats["bend_factor"] = 0.003f;
    newShader->shaderParams.floats["selection_value"] = -1.0f;
    newShader->shaderParams.vec3s["helicopter_position"] = glm::vec3(0);

    return newShader;
}

Shader* ShaderLoader::LoadMinimapShader()
{
    // Check if the name exists
    if (ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_MINIMAP) != nullptr)
        return nullptr;
    
    Shader *newShader = LoadShader(ShaderResourceManager::SHADER_MINIMAP,
            "Shaders/Minimap/Minimap.VS.glsl",
            "Shaders/Minimap/Minimap.FS.glsl",
            true);
    
    ShaderResourceManager::AddShader(ShaderResourceManager::SHADER_MINIMAP, newShader);
    newShader->shaderParams.ints["draw_heightmap"] = 0;
    newShader->shaderParams.floats["time_of_day"] = 1.0f;

    return newShader;
}

Shader* ShaderLoader::LoadViewDepthTextureShader()
{
    // Check if the name exists
    if (ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_VIEW_DEPTH_TEXTURE) != nullptr)
        return nullptr;
    
    Shader *newShader = LoadShader(ShaderResourceManager::SHADER_VIEW_DEPTH_TEXTURE,
            "Shaders/Shadows/ViewDepthTexture.VS.glsl",
            "Shaders/Shadows/ViewDepthTexture.FS.glsl",
            true);
    
    ShaderResourceManager::AddShader(ShaderResourceManager::SHADER_VIEW_DEPTH_TEXTURE, newShader);

    return newShader;
}

Shader* ShaderLoader::LoadViewColorTextureShader()
{
    // Check if the name exists
    if (ShaderResourceManager::GetShader(ShaderResourceManager::SHADER_VIEW_COLOR_TEXTURE) != nullptr)
        return nullptr;
    
    Shader *newShader = LoadShader(ShaderResourceManager::SHADER_VIEW_COLOR_TEXTURE,
            "Shaders/Shadows/ViewColorTexture.VS.glsl",
            "Shaders/Shadows/ViewColorTexture.FS.glsl",
            true);
    
    ShaderResourceManager::AddShader(ShaderResourceManager::SHADER_VIEW_COLOR_TEXTURE, newShader);

    return newShader;
}

Shader* ShaderLoader::LoadShader(
    const std::string& shaderName,
    const std::string& vertexShaderPath,
    const std::string& fragmentShaderPath,
    bool useAssetsFolder)
{
    // Check if the name exists
    if (ShaderResourceManager::GetShader(shaderName) != nullptr)
        return nullptr;
    
    // Set the source directory
    const string sourceTextureDir = useAssetsFolder ?
        PATH_JOIN(FileSystem::rootDirectory, ENGINE_PATH::ASSETS) :
        PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::SHADERS);

    // Create a shader program for drawing face polygon with the color of the normal
    Shader* newShader = new Shader(shaderName);
    newShader->AddShader(PATH_JOIN(sourceTextureDir, vertexShaderPath), GL_VERTEX_SHADER);
    newShader->AddShader(PATH_JOIN(sourceTextureDir, fragmentShaderPath), GL_FRAGMENT_SHADER);
    newShader->CreateAndLink();
    
    ShaderResourceManager::AddShader(shaderName, newShader);

    return newShader;
}
