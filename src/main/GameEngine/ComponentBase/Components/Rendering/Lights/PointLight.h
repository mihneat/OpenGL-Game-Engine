#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/Light.h"

namespace component
{
    SERIALIZE_CLASS
    class PointLight : public Light
    {
        MARK_SERIALIZABLE
        
    public:
        PointLight(transform::Transform* transform);
        virtual ~PointLight() { }

    protected:
    };
}
