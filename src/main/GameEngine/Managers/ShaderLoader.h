#pragma once

#include "main/GameEngine/GameEngine.h"
#include <iostream>
#include <unordered_map>

#include "main/GameEngine/Systems/Rendering/Shader.h"

namespace m1 {
    class GameEngine;
}

namespace loaders
{
    class ShaderLoader
    {
    private:
        ShaderLoader() { }
        virtual ~ShaderLoader() { }

    public:
        static void InitShaders();
        static void LoadShaderFromFile(std::string shaderPath);
        static void LoadShaderFromType(rendering::ShaderType type);
        static rendering::Shader* LoadShader(
            const std::string& shaderName,
            const std::string& vertexShaderPath,
            const std::string& fragmentShaderPath,
            bool useAssetsFolder = false
        );

    private:
        static rendering::Shader* LoadStandardShader();
        static rendering::Shader* LoadHeightMapShader();
        static rendering::Shader* LoadSkyboxShader();
        static rendering::Shader* LoadTreeShader();
    };
}   // namespace loaders
