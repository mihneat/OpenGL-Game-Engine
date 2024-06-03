#pragma once

#include "main/GameEngine/GameEngine.h"
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
        enum TextureName {
            Snow,
            Present,
            TreeCorona,
            TreeBark,
            Rock,
            LightPole,
            Player,
            Player2,
            Player3
        };

        static void InitTexturesMap();
        static void InitTextures();
        static void LoadTexture(const std::string& texId, const std::string& texPath);
        static std::string GetTextureNameFromEnum(TextureName texName);
        static int GetTextureIdByName(const std::string& textureName);

    private:
        static std::unordered_map<TextureName, std::string> texToId;
        static std::unordered_map<std::string, Texture2D*> mapTextures;
        static bool isInitialized;
    };
}   // namespace loaders
