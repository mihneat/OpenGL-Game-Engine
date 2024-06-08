#pragma once

#include <glm/fwd.hpp>

#include "Material.h"
#include "MaterialOverrides.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"

namespace rendering
{
    class RenderingSystem
    {
    public:
        void Init(transform::Transform* hierarchy, gfxc::TextRenderer* textRenderer);
        void Render(transform::Transform* hierarchy, component::Camera* cam, const glm::ivec2 resolution);
        
    private:
        void SetGlobalUniforms(
            ShaderBase* shader,
            const glm::mat4& modelMatrix,
            bool renderUI,
            std::string texture,
            glm::vec2 texScale,
            component::Camera* cam,
            glm::ivec2 resolution
        );
        void SetUniforms(const Material* material, const MaterialOverrides* materialOverrides);
        void SetIntUniforms(const Material* material, const MaterialOverrides* materialOverrides);
        void SetFloatUniforms(const Material* material, const MaterialOverrides* materialOverrides);
        void SetVec2Uniforms(const Material* material, const MaterialOverrides* materialOverrides);
        void SetVec3Uniforms(const Material* material, const MaterialOverrides* materialOverrides);
    };
}
