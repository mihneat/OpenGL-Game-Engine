#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/Systems/Rendering/Material.h"

namespace component
{
    class SteepShaderParams : public Component
    {
    public:
        SteepShaderParams(transform::Transform* transform, rendering::Material* groundMat) : Component(transform),
            offset(glm::vec2()), player(nullptr), previousPlayerPos(glm::vec3()), groundMat(groundMat) { }
        ~SteepShaderParams() { }

        void Start();
        void Update(const float deltaTime);

        glm::vec2 offset;

    protected:
        transform::Transform* player;
        glm::vec3 previousPlayerPos;
        rendering::Material* groundMat;
    };
}   // namespace component
