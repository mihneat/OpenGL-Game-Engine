#include "main/GameEngine/ComponentBase/Components/Logic/Player/GroundStick.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

#include "main/GameEngine/Managers/GameInstance.h"
#include "main/GameEngine/Managers/InputManager.h"

using namespace std;
using namespace component;
using namespace component;
using namespace transform;

void GroundStick::Start()
{
	player = transform->GetTransformByTag("Player");
	if (!player) {
		cerr << "Object with tag 'Player' not found.\n";
		return;
	}

	offset = transform->GetWorldPosition() - player->GetWorldPosition();
	// cout << transform->GetWorldPosition() << " " << player->GetWorldPosition() << "\n";
}

void GroundStick::Update(const float deltaTime)
{
	if (managers::GameInstance::Get()->GetComponent<GameManager>()->GetGameState() == GameManager::Ended) {
		return;
	}

	transform->SetLocalPosition(player->GetWorldPosition() + offset);
}
