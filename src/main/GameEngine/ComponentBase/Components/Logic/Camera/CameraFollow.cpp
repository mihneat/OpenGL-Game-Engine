#include "main/GameEngine/ComponentBase/Components/Logic/Camera/CameraFollow.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace component;
using namespace component;
using namespace transform;

void CameraFollow::Start()
{
	mainCam = transform->GetComponent<Camera>();

	// Get the forward position
	forwardOffset = followTarget->forward * forwardFollowDistance;

	// Compute the initial camera starting position
	const float angle = angleScale * glm::pi<float>();

	// Get vector to target
	glm::vec3 vecToTarget = glm::normalize(glm::vec3(
		glm::rotate(glm::mat4(1.0f), angle, followTarget->right) * glm::vec4(followTarget->forward, 1.0f)
	)) * distanceToTarget;

	// Get the camera initial position
	glm::vec3 startingPos = followTarget->GetWorldPosition() + forwardOffset - vecToTarget;

	// Set the camera to the first position
	mainCam->Set(startingPos, followTarget->GetWorldPosition() + forwardOffset, glm::vec3_up);
}

void CameraFollow::LateUpdate(const float deltaTime)
{
	// Check if the game has ended
	if (GameManager::GetInstance()->GetGameState() == GameManager::Ended) {
		return;
	}

	// Calculate the angle of the camera
	const float angle = angleScale * glm::pi<float>();

	// Get vector to target
	glm::vec3 vecToTarget = glm::normalize(glm::vec3(
		glm::rotate(glm::mat4(1.0f), angle, followTarget->right) * glm::vec4(followTarget->forward, 1.0f)
	)) * distanceToTarget * (isRetroCam ? 0.3f : 1.0f);

	// Get retro vector to target
	const float retroAngle = retroAngleScale * glm::pi<float>();

	// Get retro vector to target
	glm::vec3 retroVecToTarget = glm::normalize(glm::vec3(
		glm::rotate(glm::mat4(1.0f), retroAngle, followTarget->right) * glm::vec4(followTarget->forward, 1.0f)
	)) * distanceToTarget;

	// Calculate and set the new camera position
	glm::vec3 camLerpedPosition = glm::mix(
		transform->GetLocalPosition(), 
		followTarget->GetWorldPosition() + forwardOffset * (isRetroCam ? 0.3f : 1.0f) - vecToTarget, // (isRetroCam ? retroVecToTarget : vecToTarget),
		 glm::clamp(1.0f, 0.0f, 1.0f));  // 0.5f * deltaTime * 100.0f // (isRetroCam ? 0.5f : 0.07f) * deltaTime * 100.0f
	mainCam->Set(camLerpedPosition, followTarget->GetWorldPosition() + forwardOffset * (isRetroCam ? 0.2f : 1.0f), glm::vec3_up);
}

void CameraFollow::KeyPress(const int key, const int mods)
{
	if (key == GLFW_KEY_SPACE) {
		isRetroCam = true;
	}
}

void CameraFollow::KeyRelease(const int key, const int mods)
{
	if (key == GLFW_KEY_SPACE) {
		isRetroCam = false;
	}
}
