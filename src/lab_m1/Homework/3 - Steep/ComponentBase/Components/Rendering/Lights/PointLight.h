#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Lights/Light.h"

namespace component
{
    class PointLight : public Light
    {
    public:
        PointLight(transform::Transform* transform);
        virtual ~PointLight() { }

    protected:
    };
}
