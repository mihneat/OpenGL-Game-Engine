#pragma once
#include <string>
#include <unordered_map>

#include "core/gpu/shader.h"
#include "main/GameEngine/Managers/ShaderLoader.h"

namespace rendering
{
    class ShaderResourceManager
    {
    public:
        static Shader* GetShader(const std::string& shaderName);
    
    private:
        static void AddShader(const std::string& shaderName, Shader* shader);

    public:
        static const std::string SHADER_STANDARD;
        static const std::string SHADER_SIMPLE;
        static const std::string SHADER_COLOR;
    
    private:
        static std::unordered_map<std::string, Shader*> shaders;

        friend class loaders::ShaderLoader;
    };
}
