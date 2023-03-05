#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Shading/SteepShaderParams.h"

#include <iostream>

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void SteepShaderParams::Start()
{
	player = transform->GetTransformByTag("Player");
	previousPlayerPos = player->GetWorldPosition();
}

void SteepShaderParams::Update(const float deltaTime)
{
	if (GameManager::GetInstance()->GetGameState() == GameManager::Ended) {
		return;
	}

	glm::vec3 movement = player->GetWorldPosition() - previousPlayerPos;
	offset += glm::vec2(-movement.x, movement.z);

	previousPlayerPos = player->GetWorldPosition();

	GameManager::GetInstance()->GetSceneReference()->playerOffset = offset;
}
