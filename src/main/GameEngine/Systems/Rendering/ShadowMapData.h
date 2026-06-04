#pragma once

#include <glm/fwd.hpp>

#include <vector>

namespace rendering
{
    struct ShadowMapData
    {
        std::vector<glm::mat4> lightViewMatrices;
        std::vector<glm::mat4> lightProjectionMatrices;
        std::vector<int> depthTextureIds;
        std::vector<glm::vec2> zPlaneFractions;
        float zFar;
    };
}
