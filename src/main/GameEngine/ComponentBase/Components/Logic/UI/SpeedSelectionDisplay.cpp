#include "main/GameEngine/ComponentBase/Components/Logic/UI/SpeedSelectionDisplay.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace component;
using namespace component;
using namespace transform;

void SpeedSelectionDisplay::Start()
{
	textRenderer = transform->GetComponent<TextRenderer>();
	prevTextRenderer = transform->GetChild(0)->GetComponent<TextRenderer>();
	currTextRenderer = transform->GetChild(1)->GetComponent<TextRenderer>();
	nextTextRenderer = transform->GetChild(2)->GetComponent<TextRenderer>();
}

void SpeedSelectionDisplay::Update(const float deltaTime)
{
	textRenderer->SetActive(GameManager::GetInstance()->GetGameState() == GameManager::Start);
	prevTextRenderer->SetActive(GameManager::GetInstance()->GetGameState() == GameManager::Start);
	currTextRenderer->SetActive(GameManager::GetInstance()->GetGameState() == GameManager::Start);
	nextTextRenderer->SetActive(GameManager::GetInstance()->GetGameState() == GameManager::Start);

	// Get the current game speed
	switch (GameManager::GetInstance()->GetGameSpeed()) {
	case GameManager::Snail:
		EmptyText(prevTextRenderer);
		SetTextValues(currTextRenderer, GameManager::Snail);
		SetTextValues(nextTextRenderer, GameManager::Slow, true);
		break;

	case GameManager::Slow:
		SetTextValues(prevTextRenderer, GameManager::Snail, true);
		SetTextValues(currTextRenderer, GameManager::Slow);
		SetTextValues(nextTextRenderer, GameManager::Medium, true);
		break;

	case GameManager::Medium:
		SetTextValues(prevTextRenderer, GameManager::Slow, true);
		SetTextValues(currTextRenderer, GameManager::Medium);
		SetTextValues(nextTextRenderer, GameManager::Fast, true);
		break;

	case GameManager::Fast:
		SetTextValues(prevTextRenderer, GameManager::Medium, true);
		SetTextValues(currTextRenderer, GameManager::Fast);
		SetTextValues(nextTextRenderer, GameManager::Cheetah, true);
		break;

	case GameManager::Cheetah:
		SetTextValues(prevTextRenderer, GameManager::Fast, true);
		SetTextValues(currTextRenderer, GameManager::Cheetah);
		SetTextValues(nextTextRenderer, GameManager::LightningMcQueen, true);
		break;

	case GameManager::LightningMcQueen:
		SetTextValues(prevTextRenderer, GameManager::Cheetah, true);
		SetTextValues(currTextRenderer, GameManager::LightningMcQueen);
		EmptyText(nextTextRenderer);
		break;
	}
}

void SpeedSelectionDisplay::SetTextValues(TextRenderer* textRenderer, int gameSpeed, bool fade)
{
	static const string snailText = "Snail";
	static const string slowText = "Slow";
	static const string mediumText = "Medium";
	static const string fastText = "Fast";
	static const string cheetahText = "Cheetah";
	static const string lightningText = "Lightning McQueen";

	static const glm::vec4 snailColor = glm::vec4(36, 250, 161, 255) / 255.0f;
	static const glm::vec4 slowColor = glm::vec4(153, 250, 70, 255) / 255.0f;
	static const glm::vec4 mediumColor = glm::vec4(250, 212, 100, 255) / 255.0f;
	static const glm::vec4 fastColor = glm::vec4(250, 164, 92, 255) / 255.0f;
	static const glm::vec4 cheetahColor = glm::vec4(250, 110, 74, 255) / 255.0f;
	static const glm::vec4 lightningColor = glm::vec4(195, 51, 50, 255) / 255.0f;

	switch (gameSpeed) {
	case GameManager::Snail:
		textRenderer->SetText(snailText);
		textRenderer->SetColor(snailColor * (fade ? 0.4f : 1.0f));
		break;

	case GameManager::Slow:
		textRenderer->SetText(slowText);
		textRenderer->SetColor(slowColor * (fade ? 0.4f : 1.0f));
		break;

	case GameManager::Medium:
		textRenderer->SetText(mediumText);
		textRenderer->SetColor(mediumColor * (fade ? 0.4f : 1.0f));
		break;

	case GameManager::Fast:
		textRenderer->SetText(fastText);
		textRenderer->SetColor(fastColor * (fade ? 0.4f : 1.0f));
		break;

	case GameManager::Cheetah:
		textRenderer->SetText(cheetahText);
		textRenderer->SetColor(cheetahColor * (fade ? 0.4f : 1.0f));
		break;

	case GameManager::LightningMcQueen:
		textRenderer->SetText(lightningText);
		textRenderer->SetColor(lightningColor * (fade ? 0.4f : 1.0f));
		break;
	}
}

void SpeedSelectionDisplay::EmptyText(TextRenderer* textRenderer)
{
	textRenderer->SetText("");
}
