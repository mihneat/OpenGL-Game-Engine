#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/Systems/Rendering/Material.h"

namespace component
{
    SERIALIZE_CLASS
    class SteepShaderParams : public Component
    {
        MARK_SERIALIZABLE
        
    public:
        SteepShaderParams(transform::Transform* transform, rendering::Material* groundMat = nullptr) : Component(transform), groundMat(groundMat) { }
        ~SteepShaderParams() { }

        void Start();
        void Update(const float deltaTime);

    protected:
        transform::Transform* player = nullptr;
        glm::vec3 previousPlayerPos = glm::vec3();
        rendering::Material* groundMat = nullptr;

    private:
        glm::vec2 offset = glm::vec2();
        
    };
}   // namespace component
