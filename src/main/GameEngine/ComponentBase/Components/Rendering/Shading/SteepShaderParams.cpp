#include "main/GameEngine/ComponentBase/Components/Rendering/Shading/SteepShaderParams.h"

#include <iostream>

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/Managers/GameInstance.h"

using namespace std;
using namespace component;
using namespace component;
using namespace transform;

void SteepShaderParams::Start()
{
	player = transform->GetTransformByTag("Player");
	previousPlayerPos = player->GetWorldPosition();
}

void SteepShaderParams::Update(const float deltaTime)
{
	if (managers::GameInstance::Get()->GetComponent<GameManager>()->GetGameState() == GameManager::Ended) {
		return;
	}

	const glm::vec3 movement = player->GetWorldPosition() - previousPlayerPos;
	offset += glm::vec2(-movement.x, movement.z);

	previousPlayerPos = player->GetWorldPosition();

	// Modify the material
	groundMat->UpdateVec2("scroll_amount", offset);
}
