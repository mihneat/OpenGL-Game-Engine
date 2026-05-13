#include "main/GameEngine/Managers/TextureLoader.h"

#include "core/managers/resource_path.h"
#include "main/GameEngine/Serialization/Database.h"
#include "main/GameEngine/Systems/FileSystem.h"

using namespace std;
using namespace loaders;

bool TextureLoader::isInitialized;
std::unordered_map<std::string, rendering::Texture*> TextureLoader::mapTextures;


void TextureLoader::InitTextures()
{
    isInitialized = true;

    LoadTexture("Snow2", "my_textures/steep/snow/snow2.png");
    LoadTexture("Present", "my_textures/steep/present/present.png");
    LoadTexture("TreeCorona", "my_textures/steep/leaf/leaf.png");
    LoadTexture("TreeBark", "my_textures/steep/bark/bark.png");
    LoadTexture("Rock", "my_textures/steep/rock/rock.jpg");
    LoadTexture("LightPole", "my_textures/steep/metal/metal.jpg");
    LoadTexture("Player", "my_textures/steep/player/player.png");
    LoadTexture("Player2", "my_textures/steep/player/player2.jpg");
    LoadTexture("Player3", "my_textures/steep/player/player3.png");
    LoadTexture("Heart", "my_textures/steep/heart/heart.png");

    LoadTexture("Ground", "ground.jpg");
    LoadTexture("Snow", "Textures/snow.jpg", true);
    LoadTexture("HeightMap", "Textures/noise.png", true);
    LoadTexture("HeightMap2", "Textures/heightmap2.png", true);
    LoadTexture("Water", "Textures/water.jpg", true);
    LoadTexture("White", "Textures/white.png", true);
    LoadTexture("Space1", "Textures/space1.jpg", true);
    LoadTexture("Space2", "Textures/space2.png", true);
    LoadTexture("Space3", "Textures/space3.jpg", true);
    LoadTexture("Ground2", "Textures/ground2.jpg", true);
    LoadTexture("TreeBark2", "Textures/treeBark2.jpg", true);
    LoadTexture("TreeCorona2", "Textures/corona2.png", true);
    LoadTexture("TreeCorona3", "Textures/corona3.jpg", true);

    LoadTexture("Gravel Albedo", "Textures/gravel_albedo.jpg", true);
    LoadTexture("Gravel Normal", "Textures/gravel_normal.jpg", true);
    LoadTexture("Snow Albedo", "Textures/snow_albedo.jpg", true);
    LoadTexture("Snow Normal", "Textures/snow_normal.jpg", true);
    LoadTexture("Propeller Albedo", "Textures/propeller_albedo.jpg", true);
    LoadTexture("Propeller Normal", "Textures/propeller_normal.jpg", true);
    LoadTexture("Helicopter Albedo", "Textures/helicopter_albedo.jpg", true);
    LoadTexture("Helicopter Normal", "Textures/helicopter_normal.jpg", true);
    
}

void TextureLoader::LoadTexture(const std::string& texId, const std::string& texPath, bool useAssetsFolder)
{
    static int i = 200000;
    ++i;
    
    // Set the source directory
    const string sourceTextureDir = useAssetsFolder ?
        PATH_JOIN(FileSystem::rootDirectory, ENGINE_PATH::ASSETS) :
        PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::TEXTURES);

    // Load the texture
    rendering::Texture* texture = new rendering::Texture();
    texture->Load2D(PATH_JOIN(sourceTextureDir, texPath).c_str(), GL_REPEAT);
    mapTextures[texId] = texture;
    
    Database::AddEntry({to_string(i), texId, "Texture", texture});
}

int TextureLoader::GetTextureIdByName(const std::string& textureName)
{
    if (!isInitialized) {
        InitTextures();
    }

    if (mapTextures.find(textureName) != mapTextures.end()) {
        return mapTextures[textureName]->GetTextureID();
    }

    return -1;
}

rendering::Texture* TextureLoader::GetTextureByName(const std::string& textureName)
{
    if (!isInitialized) {
        InitTextures();
    }

    if (mapTextures.find(textureName) != mapTextures.end()) {
        return mapTextures[textureName];
    }

    return nullptr;
}
