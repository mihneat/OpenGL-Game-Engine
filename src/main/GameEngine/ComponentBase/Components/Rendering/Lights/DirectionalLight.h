#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/Light.h"

namespace component
{
    class DirectionalLight : public Light
    {
    public:
        DirectionalLight(transform::Transform* transform);
        virtual ~DirectionalLight() { }

    protected:
    };
}
