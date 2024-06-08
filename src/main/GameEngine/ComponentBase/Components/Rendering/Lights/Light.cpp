#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/Light.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

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

void Light::Update(const float deltaTime)
{
	LightManager::lights[lightIndex].intensity = intensity;
	LightManager::lights[lightIndex].position = position;
	LightManager::lights[lightIndex].color = glm::vec3(color);
	LightManager::lights[lightIndex].direction = direction;
}

void Light::ChangeIntensity(float newIntensity)
{
	this->intensity = newIntensity;
}

void Light::ChangePosition(glm::vec3 newPosition)
{
	this->position = newPosition;
}

void Light::ChangeColor(glm::vec4 newColor)
{
	this->color = newColor;
}

void Light::ChangeDirection(glm::vec3 newDirection)
{
	this->direction = newDirection;
}
