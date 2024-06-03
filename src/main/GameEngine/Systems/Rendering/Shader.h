#pragma once

#include <string>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "ShaderParams.h"
#include "core/gpu/shader.h"

namespace rendering
{
    // TODO: Temporary solution until shader file reading is implemented
    enum ShaderType
    {
        Standard,
        Simple,
        Color,
        VertexNormal,
        VertexColor,
    };
    
    class Shader: public ShaderBase
    {
    public:
        Shader(const std::string& name) : ShaderBase(name) { }
        // Shader(const std::string& name) { ShaderBase(name); }

        ShaderParams shaderParams;

        // TODO: Do I need these?
        // void SetInt(const std::string& name, const int value) { ints[name] = value; }
        // void SetFloat(const std::string& name, const float value) { floats[name] = value; }
        // void SetVec3(const std::string& name, const glm::vec3 value) { vec3s[name] = value; }
        //
        // bool HasInt(const std::string& name) { return ints.find(name) != ints.end(); }
        // bool HasFloat(const std::string& name) { return floats.find(name) != floats.end(); }
        // bool HasVec3(const std::string& name) { return vec3s.find(name) != vec3s.end(); }
        //
        // int GetInt(const std::string& name) { return ints[name]; }
        // float GetFloat(const std::string& name) { return floats[name]; }
        // glm::vec3 GetVec3(const std::string& name) { return vec3s[name]; }

       friend class RenderingSystem;
    };
}
