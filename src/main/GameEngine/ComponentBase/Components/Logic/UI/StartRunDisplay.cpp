#include "main/GameEngine/ComponentBase/Components/Logic/UI/StartRunDisplay.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

#include "main/GameEngine/Managers/GameInstance.h"

using namespace std;
using namespace component;
using namespace component;
using namespace transform;

void StartRunDisplay::Start()
{
	textRenderer = transform->GetComponent<TextRenderer>();
}

void StartRunDisplay::Update(const float deltaTime)
{
	textRenderer->SetActive(managers::GameInstance::Get()->GetComponent<GameManager>()->GetGameState() == GameManager::Start);
}
