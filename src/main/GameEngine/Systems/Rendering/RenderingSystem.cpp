#include "RenderingSystem.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "MeshResourceManager.h"
#include "ShaderResourceManager.h"
#include "main/GameEngine/GameEngine.h"
#include "main/GameEngine/Systems/Editor/EditorRuntimeSettings.h"

using namespace rendering;
using namespace loaders;

void RenderingSystem::Render(transform::Transform* hierarchy, gfxc::TextRenderer* textRenderer, component::Camera* cam,
                             const glm::ivec2 resolution, bool renderText)
{
    // Clear rendering state
    meshesByShader.clear();
    LightManager::ClearValues();
    
    // Render the meshes and the lights
    m1::GameEngine::ApplyToComponents(hierarchy, [this, cam](component::Component* component) {
        component::Light* light = dynamic_cast<component::Light*>(component);
        if (light != nullptr) {
            light->UpdateLightValues();
        }
        
        component::MeshRenderer* meshRenderer = dynamic_cast<component::MeshRenderer*>(component);
        if (meshRenderer != nullptr) {
            // Initialize the mesh
            meshRenderer->Init();

            // Check if the object has a mesh
            if (meshRenderer->meshType == component::MeshRenderer::None) return;
            
            // Check the layer
            if (!cam->IsLayerRendered(meshRenderer->layer)) return;

            // Check if it's rendered during debug mode
            if (meshRenderer->debugOnly && !EditorRuntimeSettings::debugMode) return;
            
            // Check if the renderer has a material
            const Material* material = meshRenderer->GetMaterial();
            if (material == nullptr) return;
            
            this->meshesByShader[material->shader].push_back(meshRenderer);
        }
    });

    // Iterate through all the meshes, by their shader
    for (auto& shaderMeshes : meshesByShader)
    {
        Shader* shader = shaderMeshes.first;
        shader->Use();

        SetGlobalUniforms(shader, cam);

        for (auto meshRenderer : shaderMeshes.second)
        {
            const Material* material = meshRenderer->GetMaterial();

            // Set the uniforms
            SetLocalUniforms(material->shader, meshRenderer, cam, resolution);
            
            SetShaderSpecificUniforms(material, meshRenderer->GetMaterialOverrides());
            
            // Render the mesh
            const Mesh *mesh = MeshResourceManager::meshes[std::to_string(meshRenderer->meshType)];
            mesh->Render();
        }
    }

    // Render the text
    // TODO: Temporary variable until making a full canvas system
    if (renderText)
        m1::GameEngine::ApplyToComponents(hierarchy, [this, textRenderer](component::Component* component) {
            component::TextRenderer* text = dynamic_cast<component::TextRenderer*>(component);
            if (text != nullptr) {
                text->Init(textRenderer);
                text->textRenderer->RenderText(text->text, text->position.x, text->position.y, text->scale, text->color);
            }
        });
}

void RenderingSystem::SetGlobalUniforms(
    ShaderBase* shader,
    component::Camera* cam
)
{
    const GLint eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3fv(eye_position, 1, glm::value_ptr(cam->transform->GetWorldPosition()));

    // Send light information
    static std::vector<std::string> lightIsUsedStrings;
    static std::vector<std::string> lightTypeStrings;
    static std::vector<std::string> lightIntensityStrings;
    static std::vector<std::string> lightIsPositionStrings;
    static std::vector<std::string> lightIsColorStrings;
    static std::vector<std::string> lightIsDirectionStrings;
    if (lightIsUsedStrings.empty())
    {
        for (int i = 0; i < LightManager::maxLights; ++i)
        {
            lightIsUsedStrings.push_back(std::string("lights[").append(std::to_string(i)).append("].isUsed"));
            lightTypeStrings.push_back(std::string("lights[").append(std::to_string(i)).append("].type"));
            lightIntensityStrings.push_back(std::string("lights[").append(std::to_string(i)).append("].intensity"));
            lightIsPositionStrings.push_back(std::string("lights[").append(std::to_string(i)).append("].position"));
            lightIsColorStrings.push_back(std::string("lights[").append(std::to_string(i)).append("].color"));
            lightIsDirectionStrings.push_back(std::string("lights[").append(std::to_string(i)).append("].direction"));
        } 
    }
    
    int cnt = 0;
    for (int i = 0; i < LightManager::maxLights; ++i)
    {
        if (LightManager::lights[i].isUsed == true) {
            ++cnt;
        }
        
        {
            const GLint location = glGetUniformLocation(shader->program, lightIsUsedStrings[i].c_str());
            glUniform1i(location, LightManager::lights[i].isUsed);
        }

        {
            const GLint location = glGetUniformLocation(shader->program, lightTypeStrings[i].c_str());
            glUniform1i(location, LightManager::lights[i].type);
        }

        {
            const GLint location = glGetUniformLocation(shader->program, lightIntensityStrings[i].c_str());
            glUniform1f(location, LightManager::lights[i].intensity);
        }

        {
            const GLint location = glGetUniformLocation(shader->program, lightIsPositionStrings[i].c_str());
            glUniform3fv(location, 1, glm::value_ptr(LightManager::lights[i].position));
        }

        {
            const GLint location = glGetUniformLocation(shader->program, lightIsColorStrings[i].c_str());
            glUniform3fv(location, 1, glm::value_ptr(LightManager::lights[i].color));
        }

        {
            const GLint location = glGetUniformLocation(shader->program, lightIsDirectionStrings[i].c_str());
            glUniform3fv(location, 1, glm::value_ptr(LightManager::lights[i].direction));
        }
    }

}

void RenderingSystem::SetLocalUniforms(
    ShaderBase* shader,
    component::MeshRenderer* meshRenderer,
    component::Camera* cam,
    glm::ivec2 resolution
)
{
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE,
        meshRenderer->renderInWorldSpace
            ? glm::value_ptr(cam->GetViewMatrix())
            : glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, 
        meshRenderer->renderInWorldSpace
            ? glm::value_ptr(cam->GetProjectionMatrix())
            : glm::value_ptr(glm::ortho(0.0f, static_cast<float>(resolution.x),
                    0.0f, static_cast<float>(resolution.y), 0.01f, 500.0f)));

    glm::mat4 modelMatrix = meshRenderer->transform->GetModelMatrix() * transform::Transform::GetScalingMatrix(meshRenderer->meshScale);
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // TODO: Find a better way to send multiple textures
    // Send texture data
    if (meshRenderer->texture != nullptr)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, meshRenderer->texture->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_color"), 1);
        glUniform1i(glGetUniformLocation(shader->program, "use_texture"), 1);
    }
    else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(glGetUniformLocation(shader->program, "use_texture"), 0);
    }

    // Send texture scale
    glUniform2fv(glGetUniformLocation(shader->program, "tex_scale"), 1, glm::value_ptr(meshRenderer->texScale));

    // Send mesh color
    glUniform4fv(glGetUniformLocation(shader->program, "mesh_color"), 1, glm::value_ptr(meshRenderer->color));
}

void RenderingSystem::SetShaderSpecificUniforms(const Material* material, const MaterialOverrides* materialOverrides)
{
    // Add all params
    SetIntUniforms(material, materialOverrides);
    SetFloatUniforms(material, materialOverrides);
    SetVec2Uniforms(material, materialOverrides);
    SetVec3Uniforms(material, materialOverrides);
}

void RenderingSystem::SetIntUniforms(const Material* material, const MaterialOverrides* materialOverrides)
{
    for (const auto& intParam : material->params.ints)
    {
        // Extract the param name
        const std::string &paramName = intParam.first;
        
        // Get the value of the param
        int value = intParam.second;
        if (material != nullptr && material->params.ints.find(paramName) != material->params.ints.end())
            value = material->params.ints.at(paramName);
        if (materialOverrides != nullptr && materialOverrides->ints.find(paramName) != materialOverrides->ints.end())
            value = materialOverrides->ints.at(paramName);

        // Set the uniform
        const GLint intUniform = glGetUniformLocation(material->shader->program, paramName.c_str());
        glUniform1i(intUniform, value);
    }
}

void RenderingSystem::SetFloatUniforms(const Material* material, const MaterialOverrides* materialOverrides)
{
    for (const auto& floatParam : material->params.floats)
    {
        // Extract the param name
        const std::string &paramName = floatParam.first;
        
        // Get the value of the param
        float value = floatParam.second;
        if (material != nullptr && material->params.floats.find(paramName) != material->params.floats.end())
            value = material->params.floats.at(paramName);
        if (materialOverrides != nullptr && materialOverrides->floats.find(paramName) != materialOverrides->floats.end())
            value = materialOverrides->floats.at(paramName);

        // Set the uniform
        const GLint floatUniform = glGetUniformLocation(material->shader->program, paramName.c_str());
        glUniform1f(floatUniform, value);
    }
}

void RenderingSystem::SetVec2Uniforms(const Material* material, const MaterialOverrides* materialOverrides)
{
    for (const auto& vec2Param : material->shader->shaderParams.vec2s)
    {
        // Extract the param name
        const std::string &paramName = vec2Param.first;
        
        // Get the value of the param
        glm::vec2 value = vec2Param.second;
        if (material != nullptr && material->params.vec2s.find(paramName) != material->params.vec2s.end())
            value = material->params.vec2s.at(paramName);
        if (materialOverrides != nullptr && materialOverrides->vec2s.find(paramName) != materialOverrides->vec2s.end())
            value = materialOverrides->vec2s.at(paramName);

        // Set the uniform
        const GLint vec2Uniform = glGetUniformLocation(material->shader->program, paramName.c_str());
        glUniform2fv(vec2Uniform, 1, value_ptr(value));
    }
}

void RenderingSystem::SetVec3Uniforms(const Material* material, const MaterialOverrides* materialOverrides)
{
    for (const auto& vec3Param : material->shader->shaderParams.vec3s)
    {
        // Extract the param name
        const std::string &paramName = vec3Param.first;
        
        // Get the value of the param
        glm::vec3 value = vec3Param.second;
        if (material != nullptr && material->params.vec3s.find(paramName) != material->params.vec3s.end())
            value = material->params.vec3s.at(paramName);
        if (materialOverrides != nullptr && materialOverrides->vec3s.find(paramName) != materialOverrides->vec3s.end())
            value = materialOverrides->vec3s.at(paramName);

        // Set the uniform
        const GLint vec3Uniform = glGetUniformLocation(material->shader->program, paramName.c_str());
        glUniform3fv(vec3Uniform, 1, value_ptr(value));
    }
}
