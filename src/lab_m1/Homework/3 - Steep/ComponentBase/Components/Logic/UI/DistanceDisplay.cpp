#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/DistanceDisplay.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void DistanceDisplay::Start()
{
	player = transform->GetTransformByTag("Player");
	prevPos = player->GetWorldPosition();

	textRenderer = transform->GetComponent<TextRenderer>();
}

void DistanceDisplay::Update(const float deltaTime)
{
	if (GameManager::GetInstance()->GetGameState() == GameManager::Ended) {
		return;
	}

	distance += (player->GetWorldPosition() - prevPos).z;
	prevPos = player->GetWorldPosition();

	int roundedDistance = (int)((float)distance * 10.0f);

	textRenderer->SetText(std::string(std::to_string(roundedDistance / 10)).append(".").append(std::to_string(roundedDistance % 10)).append("m"));
}

void DistanceDisplay::Reset()
{
	distance = 0.0f;
	prevPos = glm::vec3();
}
