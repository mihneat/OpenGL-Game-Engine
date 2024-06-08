#include "main/GameEngine/ComponentBase/Components/Logic/UI/DistanceDisplay.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

#include "main/GameEngine/Managers/GameInstance.h"

using namespace std;
using namespace component;
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
	if (managers::GameInstance::Get()->GetComponent<GameManager>()->GetGameState() == GameManager::Ended) {
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
