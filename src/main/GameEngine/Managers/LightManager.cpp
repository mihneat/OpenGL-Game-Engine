#include "main/GameEngine/Managers/LightManager.h"

using namespace std;
using namespace loaders;

// std::vector<LightManager::LightProperties> LightManager::lights;
LightManager::LightProperties LightManager::lights[100];
constexpr int LightManager::maxLights;

void LightManager::Init()
{
	// lights = vector<LightProperties>(maxLights);
	for (int i = 0; i < maxLights; ++i) {
		lights[i].isUsed = false;
	}
}

int LightManager::AddLight(LightType lightType)
{
	// Look for an available empty light space
	for (int i = 0; i < maxLights; ++i) {
		if (lights[i].isUsed == false) {
			// Set the properties of light
			lights[i].isUsed = true;
			lights[i].type = lightType;
			lights[i].intensity = 1.0f;
			lights[i].position = glm::vec3(0.0f);
			lights[i].color = glm::vec3(1.0f);
			lights[i].direction = glm::vec3_down;

			// Return the light index
			return i;
		}
	}

	// The lights vector is full, throw an error
	throw std::runtime_error("Lights vector is full. Consider increasing maximum lights or reducing how many are in use at once.\n");
}

void LightManager::DeleteLight(int index)
{
	// Just set the light as unused
	lights[index].isUsed = false;
}
