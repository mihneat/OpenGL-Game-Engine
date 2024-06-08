#include "main/GameEngine/ComponentBase/Components/Logic/UI/HighScoreDisplay.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

#include "main/GameEngine/Managers/GameInstance.h"

using namespace std;
using namespace component;
using namespace component;
using namespace transform;

void HighScoreDisplay::Start()
{
	textRenderer = transform->GetComponent<TextRenderer>();
}

void HighScoreDisplay::Update(const float deltaTime)
{
	textRenderer->SetText(std::string(std::to_string(managers::GameInstance::Get()->GetComponent<GameManager>()->GetHighScore())));
}
