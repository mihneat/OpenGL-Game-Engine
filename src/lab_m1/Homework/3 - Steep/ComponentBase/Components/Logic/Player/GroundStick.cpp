#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Player/GroundStick.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void GroundStick::Start()
{
	player = transform->GetTransformByTag("Player");
	if (!player) {
		cerr << "Object with tag 'Player' not found.\n";
	}

	offset = transform->GetWorldPosition() - player->GetWorldPosition();
	// cout << transform->GetWorldPosition() << " " << player->GetWorldPosition() << "\n";
}

void GroundStick::Update(const float deltaTime)
{
	if (GameManager::GetInstance()->GetGameState() == GameManager::Ended) {
		return;
	}

	transform->SetLocalPosition(player->GetWorldPosition() + offset);
}
