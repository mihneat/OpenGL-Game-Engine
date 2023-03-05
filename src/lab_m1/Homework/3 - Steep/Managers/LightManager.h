#pragma once

#include "lab_m1/Homework/3 - Steep/Steep.h"
#include <iostream>
#include <vector>

namespace loaders
{
    class LightManager
    {
    private:
        LightManager() { }
        virtual ~LightManager() { }

    public:
        enum LightType {
            Directional,    // 0
            Spot,           // 1
            Point           // 2
        };

        struct LightProperties {
            bool isUsed;
            LightType type;
            float intensity;
            glm::vec3 position;
            glm::vec3 color;
            glm::vec3 direction;
        };

        static void Init();
        static int AddLight(LightType lightType);
        static void DeleteLight(int index);

        // Check with std::vector: static std::vector<LightProperties> lights;
        static LightProperties lights[100];
        static constexpr int maxLights = 100;
    };
}   // namespace loaders
