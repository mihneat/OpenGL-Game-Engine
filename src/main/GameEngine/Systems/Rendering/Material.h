﻿#pragma once
#include "Shader.h"
#include "ShaderParams.h"

namespace rendering
{
    class Material
    {
    public:
        Material(Shader* shader);
        ~Material();

        bool UpdateInt(const std::string& param, int value);
        bool UpdateFloat(const std::string& param, float value);
        bool UpdateVec2(const std::string& param, glm::vec2 value);
        bool UpdateVec3(const std::string& param, glm::vec3 value);
        
    private:
        Shader* shader;
        ShaderParams params;

        friend class RenderingSystem;
        friend class MaterialManager;
    };
}