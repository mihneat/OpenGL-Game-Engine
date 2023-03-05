#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/RunsDisplay.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void RunsDisplay::Start()
{
	textRenderer = transform->GetComponent<TextRenderer>();
}

void RunsDisplay::Update(const float deltaTime)
{
	textRenderer->SetText(std::string("Run #").append(std::to_string(GameManager::GetInstance()->GetRuns())));
}
