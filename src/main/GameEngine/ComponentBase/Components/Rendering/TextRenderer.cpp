#include "main/GameEngine/ComponentBase/Components/Rendering/TextRenderer.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace component;
using namespace transform;

TextRenderer::TextRenderer(
    Transform* transform,
    string text,
    glm::vec2 position,
    float scale,
    glm::vec4 color
) : Component(transform)
{
    this->scene = managers::GameManager::GetInstance()->GetSceneReference();
    this->text = text;
    this->position = position;
    this->scale = scale;
	this->color = color;
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::Start()
{
    // Store a reference to the scene's text renderer
    textRenderer = scene->GetTextRenderer();
}

void TextRenderer::Update(const float deltaTime)
{

}

void TextRenderer::FrameEnd()
{
    textRenderer->RenderText(text, position.x, position.y, scale, color);
}

void TextRenderer::SetText(const std::string newText)
{
    text = newText;
}

void TextRenderer::SetColor(const glm::vec4 newColor)
{
    color = newColor;
}
