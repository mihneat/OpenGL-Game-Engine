#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/UI/LifeDisplay.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void LifeDisplay::Start()
{
	player = transform->GetTransformByTag("Player");
}

void LifeDisplay::Update(const float deltaTime)
{
	// Get the player's lives
	int lives = player->GetComponent<PlayerController>()->GetLives();

	// Update the mesh renderers of the child hearts
	for (int i = 0; i < transform->GetChildCount(); ++i) {
		transform->GetChild(i)->GetComponent<MeshRenderer>()->SetActive((lives + i - transform->GetChildCount()) >= 0);
	}
}
