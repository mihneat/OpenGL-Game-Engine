#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/StartRunDisplay.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void StartRunDisplay::Start()
{
	textRenderer = transform->GetComponent<TextRenderer>();
}

void StartRunDisplay::Update(const float deltaTime)
{
	textRenderer->SetActive(GameManager::GetInstance()->GetGameState() == GameManager::Start);
}
