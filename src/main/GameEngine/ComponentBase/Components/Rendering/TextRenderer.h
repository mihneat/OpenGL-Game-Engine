#pragma once

#include "components/text_renderer.h"
#include "main/GameEngine/GameEngine.h"
#include "main/GameEngine/ComponentBase/Component.h"

namespace prefabManager
{
    class PrefabManager;
}

namespace m1
{
    class GameEngine;
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
        m1::GameEngine* scene;
        float scale;
        glm::vec4 color;
        std::string text;
        glm::vec2 position;
        gfxc::TextRenderer* textRenderer;

    };
}   // namespace component
