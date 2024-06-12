#include "main/GameEngine/Managers/TextureLoader.h"

#include "core/managers/resource_path.h"
#include "main/GameEngine/Serialization/Database.h"
#include "main/GameEngine/Systems/FileSystem.h"

using namespace std;
using namespace loaders;

bool TextureLoader::isInitialized;
std::unordered_map<TextureLoader::TextureName, std::string> TextureLoader::texToId;
std::unordered_map<std::string, rendering::Texture*> TextureLoader::mapTextures;

void TextureLoader::InitTexturesMap()
{
    isInitialized = true;

    texToId[Snow] = "Snow";
    texToId[Present] = "Present";
    texToId[TreeCorona] = "TreeCorona";
    texToId[TreeBark] = "TreeBark";
    texToId[Rock] = "Rock";
    texToId[LightPole] = "LightPole";
    texToId[Player] = "Player";
    texToId[Player2] = "Player2";
    texToId[Player3] = "Player3";
}

void TextureLoader::InitTextures()
{
    if (!isInitialized) {
        InitTexturesMap();
    }

    // scene->LoadTexture(texToId[Snow], "my_textures/steep/snow/snow.jpg");
    LoadTexture(texToId[Snow], "my_textures/steep/snow/snow2.png");
    LoadTexture(texToId[Present], "my_textures/steep/present/present.png");
    LoadTexture(texToId[TreeCorona], "my_textures/steep/leaf/leaf.png");
    LoadTexture(texToId[TreeBark], "my_textures/steep/bark/bark.png");
    LoadTexture(texToId[Rock], "my_textures/steep/rock/rock.jpg");
    LoadTexture(texToId[LightPole], "my_textures/steep/metal/metal.jpg");
    LoadTexture(texToId[Player], "my_textures/steep/player/player.jpg");
    LoadTexture(texToId[Player2], "my_textures/steep/player/player2.jpg");
    LoadTexture(texToId[Player3], "my_textures/steep/player/player3.png");
}

void TextureLoader::LoadTexture(const std::string& texId, const std::string& texPath)
{
    static int i = 200000;
    ++i;
    
    // Set the source directory
    const string sourceTextureDir = PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::TEXTURES);

    // Load the texture
    rendering::Texture* texture = new rendering::Texture();
    texture->Load2D(PATH_JOIN(sourceTextureDir, texPath).c_str(), GL_REPEAT);
    mapTextures[texId] = texture;
    
    Database::AddEntry({to_string(i), texId, "Texture", texture});
}

std::string TextureLoader::GetTextureNameFromEnum(TextureName texName)
{
    if (!isInitialized) {
        InitTextures();
    }

    if (texToId.find(texName) != texToId.end()) {
        return texToId[texName];
    }

    return "";
}

rendering::Texture* TextureLoader::GetTextureByEnum(TextureName texName)
{
    if (!isInitialized) {
        InitTextures();
    }

    if (texToId.find(texName) != texToId.end()) {
        return GetTextureByName(texToId[texName]);
    }

    return nullptr;
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
