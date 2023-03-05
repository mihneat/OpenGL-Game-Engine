#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/ScoreDisplay.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void ScoreDisplay::Start()
{
	textRenderer = transform->GetComponent<TextRenderer>();
}

void ScoreDisplay::Update(const float deltaTime)
{
	textRenderer->SetText(std::string(std::to_string(GameManager::GetInstance()->GetScore())));
}
