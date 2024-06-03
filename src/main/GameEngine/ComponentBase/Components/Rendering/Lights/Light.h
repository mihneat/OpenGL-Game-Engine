#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"

namespace component
{
    class Light : public Component
    {
    protected:
        Light(transform::Transform* transform, int lightType);
        virtual ~Light();

    public:
        virtual void ChangeIntensity(float newIntensity);
        virtual void ChangePosition(glm::vec3 newPosition);
        virtual void ChangeColor(glm::vec3 newColor);
        virtual void ChangeDirection(glm::vec3 newDirection);

    private:
        int lightIndex;
    };
}
