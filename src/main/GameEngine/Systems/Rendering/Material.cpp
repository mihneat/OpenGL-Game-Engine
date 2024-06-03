#include "Material.h"

using namespace rendering;

Material::Material(Shader* shader)
{
    this->shader = shader;

    // Copy the params over
    this->params = this->shader->shaderParams;
}

Material::~Material() = default;

bool Material::UpdateInt(const std::string& param, int value)
{
    if (params.ints.find(param) == params.ints.end())
        return false;

    params.ints[param] = value;    
    return true;
}

bool Material::UpdateFloat(const std::string& param, float value)
{
    if (params.floats.find(param) == params.floats.end())
        return false;

    params.floats[param] = value;    
    return true;
}

bool Material::UpdateVec2(const std::string& param, glm::vec2 value)
{
    if (params.vec2s.find(param) == params.vec2s.end())
        return false;

    params.vec2s[param] = value;    
    return true;
}

bool Material::UpdateVec3(const std::string& param, glm::vec3 value)
{
    if (params.vec3s.find(param) == params.vec3s.end())
        return false;

    params.vec3s[param] = value;    
    return true;
}


