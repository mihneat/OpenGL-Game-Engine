#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/Light.h"

namespace component
{
    class UpdateLightPosition : public Component
    {
    public:
        UpdateLightPosition(transform::Transform* transform) : Component(transform) { }
        virtual ~UpdateLightPosition() { }

        void Start();
        void Update(const float deltaTime);

    protected:
        component::Light* light;
    };
}
