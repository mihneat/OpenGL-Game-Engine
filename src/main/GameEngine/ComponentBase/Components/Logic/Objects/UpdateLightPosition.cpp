#include "main/GameEngine/ComponentBase/Components/Logic/Objects/UpdateLightPosition.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/Managers/LightManager.h"

#include <iostream>

using namespace std;
using namespace loaders;
using namespace component;
using namespace transform;

void UpdateLightPosition::Start()
{
	// Temporary until better type casting system :(
	light = transform->GetComponent<DirectionalLight>();
	if (!light) {
		light = transform->GetComponent<SpotLight>();
	}

	if (!light) {
		light = transform->GetComponent<PointLight>();
	}

	if (!light) {
		fprintf(stderr, "No light found on object.\n");
	}
}

void UpdateLightPosition::Update(const float deltaTime)
{
	light->ChangePosition(transform->GetWorldPosition());
}
