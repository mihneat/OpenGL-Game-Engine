#include "Shader.h"

using namespace rendering;

bool Shader::UpdateInt(const std::string& param, int value)
{
    if (shaderParams.ints.find(param) == shaderParams.ints.end())
        return false;

    shaderParams.ints[param] = value;    
    return true;
}

bool Shader::UpdateFloat(const std::string& param, float value)
{
    if (shaderParams.floats.find(param) == shaderParams.floats.end())
        return false;

    shaderParams.floats[param] = value;    
    return true;
}

bool Shader::UpdateVec2(const std::string& param, glm::vec2 value)
{
    if (shaderParams.vec2s.find(param) == shaderParams.vec2s.end())
        return false;

    shaderParams.vec2s[param] = value;    
    return true;
}

bool Shader::UpdateVec3(const std::string& param, glm::vec3 value)
{
    if (shaderParams.vec3s.find(param) == shaderParams.vec3s.end())
        return false;

    shaderParams.vec3s[param] = value;    
    return true;
}
