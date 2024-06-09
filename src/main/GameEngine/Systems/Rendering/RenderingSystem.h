#pragma once

#include <glm/fwd.hpp>

#include "Material.h"
#include "MaterialOverrides.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/Camera.h"

namespace component
{
    class MeshRenderer;
}

namespace rendering
{
    class RenderingSystem
    {
    public:
        void Init(transform::Transform* hierarchy, gfxc::TextRenderer* textRenderer);
        void Render(transform::Transform* hierarchy, component::Camera* cam, const glm::ivec2 resolution, bool renderText = true);
        
    private:
        void SetGlobalUniforms(
            ShaderBase* shader,
            component::Camera* cam
        );
        void SetLocalUniforms(ShaderBase* shader,
            component::MeshRenderer* meshRenderer,
            component::Camera* cam,
            glm::ivec2 resolution
        );
        void SetShaderSpecificUniforms(const Material* material, const MaterialOverrides* materialOverrides);
        void SetIntUniforms(const Material* material, const MaterialOverrides* materialOverrides);
        void SetFloatUniforms(const Material* material, const MaterialOverrides* materialOverrides);
        void SetVec2Uniforms(const Material* material, const MaterialOverrides* materialOverrides);
        void SetVec3Uniforms(const Material* material, const MaterialOverrides* materialOverrides);
        
        std::unordered_map<Shader*, std::vector<component::MeshRenderer*>> meshesByShader;
    };
}
