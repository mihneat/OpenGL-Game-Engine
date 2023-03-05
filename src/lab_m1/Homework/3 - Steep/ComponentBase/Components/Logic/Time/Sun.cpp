#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Time/Sun.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace utils;
using namespace managers;
using namespace component;
using namespace transform;

void Sun::Start()
{
	GameManager::GetInstance()->GetSceneReference()->timeOfDay = 1.0f;

	sunPosition = glm::normalize(glm::vec3(3.0f, 15.0f, 5.0f)) * dist;
	lightSource = transform->GetComponent<DirectionalLight>();
	lightSource->ChangeDirection(-sunPosition);
}

void Sun::Update(const float deltaTime)
{
	// 'Auto' to avoid header inclusion :3
	auto scene = GameManager::GetInstance()->GetSceneReference();

	// Remove the z coord and renormalize the vector
	glm::vec3 circle = glm::normalize(glm::vec3(sunPosition.x, sunPosition.y, 0.0f));

	// Rotate the sun
	glm::vec3 normalizedSun = glm::normalize(glm::vec3(
		glm::rotate(glm::mat4(1.0f), (circle.y < -0.4f) ? (nightSpeed * deltaTime) : (speed * deltaTime), glm::vec3_forward) * glm::vec4(sunPosition, 1)
	));
	sunPosition = normalizedSun * dist;

	// Set the sun direction
	lightSource->ChangeDirection(-sunPosition);

	scene->timeOfDay = circle.y < 0.0f ? 0.0f : circle.y;

	const glm::vec4 nightColor = glm::vec4(21, 26, 92, 255.0f) / 255.0f;
	const glm::vec4 duskColor = glm::vec4(219, 103, 42, 255.0f) / 255.0f;
	//const glm::vec4 clearSkyColor = glm::vec4(44, 191, 245, 255.0f) / 255.0f;
	const glm::vec4 clearSkyColor = glm::vec4(1.0f);

	glm::vec4 skyColor;
	if (circle.y < -0.4f) {
		skyColor = nightColor;
	}
	else if (circle.y >= -0.4f && circle.y < 0.3f) {
		skyColor = glm::mix(nightColor, duskColor, (circle.y + 0.4f) / 0.7f);
	}
	else if (circle.y >= 0.3f && circle.y < 0.8f) {
		skyColor = glm::mix(duskColor, clearSkyColor, (circle.y - 0.3f) / 0.5f);
	}
	else {
		skyColor = clearSkyColor;
	}

	// GameManager::GetInstance()->SetSkyColor(skyColor);
	lightSource->ChangeColor(skyColor);
}
