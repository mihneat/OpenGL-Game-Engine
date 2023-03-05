#include "lab_m1/Homework/3 - Steep/Managers/TextureLoader.h"

using namespace std;
using namespace loaders;


bool TextureLoader::isInitialized;
std::unordered_map<TextureLoader::TextureName, std::string> TextureLoader::texToId;

void TextureLoader::InitTexturesMap()
{
    isInitialized = true;

    texToId[Snow] = "snow";
    texToId[Present] = "present";
    texToId[TreeCorona] = "treeCorona";
    texToId[TreeBark] = "treeBark";
    texToId[Rock] = "rock";
    texToId[LightPole] = "lightPole";
    texToId[Player] = "player";
    texToId[Player2] = "player2";
    texToId[Player3] = "player3";
}

void TextureLoader::InitTextures(m1::Steep* scene)
{
    if (!isInitialized) {
        InitTexturesMap();
    }

    // scene->LoadTexture(texToId[Snow], "my_textures/steep/snow/snow.jpg");
    scene->LoadTexture(texToId[Snow], "my_textures/steep/snow/snow2.png");
    scene->LoadTexture(texToId[Present], "my_textures/steep/present/present.png");
    scene->LoadTexture(texToId[TreeCorona], "my_textures/steep/leaf/leaf.png");
    scene->LoadTexture(texToId[TreeBark], "my_textures/steep/bark/bark.png");
    scene->LoadTexture(texToId[Rock], "my_textures/steep/rock/rock.jpg");
    scene->LoadTexture(texToId[LightPole], "my_textures/steep/metal/metal.jpg");
    scene->LoadTexture(texToId[Player], "my_textures/steep/player/player.jpg");
    scene->LoadTexture(texToId[Player2], "my_textures/steep/player/player2.jpg");
    scene->LoadTexture(texToId[Player3], "my_textures/steep/player/player3.png");

}

std::string TextureLoader::GetTexture(TextureName texName)
{
    if (!isInitialized) {
        InitTexturesMap();
    }

    if (texToId.find(texName) != texToId.end()) {
        return texToId[texName];
    }

    return "";
}
