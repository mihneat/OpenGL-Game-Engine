#pragma once

#include "components/text_renderer.h"
#include "lab_m1/Homework/3 - Steep/Steep.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"

namespace prefabManager
{
    class PrefabManager;
}

namespace m1
{
    class Steep;
}

namespace component
{
    class TextRenderer : public Component
    {
    public:
        TextRenderer(
            transform::Transform* transform,
            std::string text = "",
            glm::vec2 position = glm::vec2(),
            float scale = 1.0f,
            glm::vec4 color = glm::vec4(1.0f)
        );
        ~TextRenderer();

        void Start();
        void Update(const float deltaTime);
        void FrameEnd();

        void SetText(const std::string newText);
        void SetColor(const glm::vec4 newColor);

    protected:
        m1::Steep* scene;
        float scale;
        glm::vec4 color;
        std::string text;
        glm::vec2 position;
        gfxc::TextRenderer* textRenderer;

    };
}   // namespace component
