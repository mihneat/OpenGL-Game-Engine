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
    SERIALIZE_CLASS
    class TextRenderer : public Component
    {
        MARK_SERIALIZABLE(TextRenderer)
        
    public:
        TextRenderer(
            transform::Transform* transform,
            std::string text = "",
            glm::vec2 position = glm::vec2(),
            float scale = 1.0f,
            glm::vec4 color = glm::vec4(1.0f)
        );
        ~TextRenderer();

        void Init(gfxc::TextRenderer* textRenderer);

        void SetText(const std::string newText);
        void SetColor(const glm::vec4 newColor);

    protected:
        SERIALIZE_FIELD float scale;
        SERIALIZE_FIELD glm::vec4 color;
        SERIALIZE_FIELD std::string text;
        SERIALIZE_FIELD glm::vec2 position;

    private:
        bool initialized = false;

        friend class rendering::RenderingSystem;
    };
}   // namespace component
