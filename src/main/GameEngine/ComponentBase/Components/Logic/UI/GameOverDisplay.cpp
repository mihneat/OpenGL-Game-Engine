#include "main/GameEngine/ComponentBase/Components/Logic/UI/GameOverDisplay.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

#include "main/GameEngine/Managers/GameInstance.h"

using namespace std;
using namespace component;
using namespace component;
using namespace transform;

void GameOverDisplay::Start()
{
	textRenderer = transform->GetComponent<TextRenderer>();
}

void GameOverDisplay::Update(const float deltaTime)
{
	textRenderer->SetActive(managers::GameInstance::Get()->GetComponent<GameManager>()->GetGameState() == GameManager::Ended);
}
