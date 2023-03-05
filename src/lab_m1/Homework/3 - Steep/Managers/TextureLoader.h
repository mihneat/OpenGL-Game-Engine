#pragma once

#include "lab_m1/Homework/3 - Steep/Steep.h"
#include <iostream>
#include <unordered_map>

namespace m1 {
    class Steep;
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
        static void InitTextures(m1::Steep* scene);
        static std::string GetTexture(TextureName texName);

    private:
        static std::unordered_map<TextureName, std::string> texToId;
        static bool isInitialized;
    };
}   // namespace loaders
