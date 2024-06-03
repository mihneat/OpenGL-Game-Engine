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
            const std::string& fragmentShaderPath
        );

    private:
        static rendering::Shader* LoadStandardShader();
        
    };
}   // namespace loaders
