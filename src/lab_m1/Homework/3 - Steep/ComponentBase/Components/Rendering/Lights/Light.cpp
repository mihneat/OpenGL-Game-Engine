#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Lights/Light.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace loaders;
using namespace component;
using namespace transform;

Light::Light(transform::Transform* transform, int lightType) : Component(transform)
{
	lightIndex = LightManager::AddLight((LightManager::LightType)lightType);
}

Light::~Light()
{
	// cout << "Deleting light??\n";
	LightManager::DeleteLight(lightIndex);
}

void Light::ChangeIntensity(float newIntensity)
{
	LightManager::lights[lightIndex].intensity = newIntensity;
}

void Light::ChangePosition(glm::vec3 newPosition)
{
	LightManager::lights[lightIndex].position = newPosition;
}

void Light::ChangeColor(glm::vec3 newColor)
{
	LightManager::lights[lightIndex].color = newColor;
}

void Light::ChangeDirection(glm::vec3 newDirection)
{
	LightManager::lights[lightIndex].direction = newDirection;
}
