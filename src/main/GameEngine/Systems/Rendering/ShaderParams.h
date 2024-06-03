#pragma once
#include <string>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace rendering
{
    struct ShaderParams
    {
        std::unordered_map<std::string, int> ints;
        std::unordered_map<std::string, float> floats;
        std::unordered_map<std::string, glm::vec2> vec2s;
        std::unordered_map<std::string, glm::vec3> vec3s;
    };
}
