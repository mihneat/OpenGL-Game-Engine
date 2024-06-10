#pragma once

#include <vector>
#include <unordered_set>

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include "main/GameEngine/ComponentBase/Component.h"

namespace rendering
{
    class RenderingSystem;
}

namespace component
{
    SERIALIZE_CLASS
    class Light : public Component
    {
        MARK_SERIALIZABLE
        
    protected:
        Light(transform::Transform* transform);
        virtual ~Light();

    public:
        virtual void ChangeType(int newType);
        virtual void ChangeIntensity(float newIntensity);
        virtual void ChangePosition(glm::vec3 newPosition);
        virtual void ChangeColor(glm::vec4 newColor);
        virtual void ChangeDirection(glm::vec3 newDirection);

    private:
        void UpdateLightValues() const;
        
        int lightIndex;

    protected:
        // TODO: Temporary until enums are serialized
        SERIALIZE_FIELD int type = 0;
        SERIALIZE_FIELD float intensity = 0.0f;
        SERIALIZE_FIELD glm::vec3 position = glm::vec3();
        SERIALIZE_FIELD glm::vec4 color = glm::vec4();
        SERIALIZE_FIELD glm::vec3 direction = glm::vec3();
        
        friend class rendering::RenderingSystem;
    };
}
