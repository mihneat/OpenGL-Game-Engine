#pragma once

#include "main/GameEngine/GameEngine.h"
#include "main/GameEngine/Systems/Rendering/Texture.h"
#include <iostream>
#include <unordered_map>

namespace m1 {
    class GameEngine;
}

namespace loaders
{
    class TextureLoader
    {
    private:
        TextureLoader() { }
        virtual ~TextureLoader() { }

    public:

        static void InitTextures();
        static void LoadTexture(const std::string& texId, const std::string& texPath, bool useAssetsFolder = false);
        static int GetTextureIdByName(const std::string& textureName);
        static rendering::Texture* GetTextureByName(const std::string& textureName);

    private:
        static std::unordered_map<std::string, rendering::Texture*> mapTextures;
        static bool isInitialized;
    };
}   // namespace loaders
