#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"

namespace component
{
    class SteepShaderParams : public Component
    {
    public:
        SteepShaderParams(transform::Transform* transform) : Component(transform), offset(glm::vec2()), previousPlayerPos(glm::vec3()) { }
        ~SteepShaderParams() { }

        void Start();
        void Update(const float deltaTime);

        glm::vec2 offset;

    protected:
        transform::Transform* player;
        glm::vec3 previousPlayerPos;
    };
}   // namespace component
