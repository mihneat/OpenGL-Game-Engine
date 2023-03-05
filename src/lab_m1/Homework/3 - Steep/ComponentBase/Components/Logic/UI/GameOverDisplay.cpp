#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/GameOverDisplay.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void GameOverDisplay::Start()
{
	textRenderer = transform->GetComponent<TextRenderer>();
}

void GameOverDisplay::Update(const float deltaTime)
{
	textRenderer->SetActive(GameManager::GetInstance()->GetGameState() == GameManager::Ended);
}
