#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Camera/OrthoCameraFollow.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void OrthoCameraFollow::Start()
{
	cam = transform->GetComponent<Camera>();

	// Get a window reference
	window = GameManager::GetInstance()->GetSceneReference()->GetWindow();

	isFixed = true;
	cam->Set(glm::vec3(300.0f, 2000.0f, 0.0f), glm::vec3(300.0f, 0.0f, 0.0f), -glm::vec3_forward);

	cam->SetOrtographic(fixedDimensions.x, fixedDimensions.y);
}

void OrthoCameraFollow::Update(const float deltaTime)
{
	if (isFixed) {
		// transform->SetLocalPosition(glm::vec3(0.0f, 100.0f, 580.0f));
		cam->Set(glm::vec3(30.0f, 2000.0f, 80.0f), glm::vec3(30.0f, 0.0f, 80.0f), -glm::vec3_forward);
		return;
	}

	// Check if the game has ended
	if (GameManager::GetInstance()->GetGameState() == GameManager::Ended) {
		return;
	}

	cam->Set(followTarget->GetWorldPosition() + glm::vec3(0.0f, 2000.0f, 0.0f), followTarget->GetWorldPosition(), -glm::vec3_forward);

}

void OrthoCameraFollow::InputUpdate(const float deltaTime, const int mods)
{
	// Zoom out
	if (window->KeyHold(GLFW_KEY_MINUS)) {
		// If already not following the player, can't zoom out more
		if (isFixed) {
			return;
		}

		// If zoomed out past the max, fix the camera
		if (zoom >= 1.0f) {
			isFixed = true;
			cam->SetOrtographic(fixedDimensions.x, fixedDimensions.y);
			return;
		}

		// Otherwise incrementally increase the height (zoom out)
		zoom = glm::clamp(zoom + zoomSpeed * deltaTime, 0.0f, 1.0f);
		cam->SetOrtographic(
			glm::mix(minDimensions.x, maxDimensions.x, zoom),
			glm::mix(minDimensions.y, maxDimensions.y, zoom)
		);

	} // Zoom in
	else if (window->KeyHold(GLFW_KEY_EQUAL)) {
		// If the camera is fixed, unfix it
		if (isFixed) {
			isFixed = false;
			zoom = 1.0f;
		}

		// Otherwise incrementally decrease the height (zoom in)
		zoom = glm::clamp(zoom - zoomSpeed * deltaTime, 0.0f, 1.0f);
		cam->SetOrtographic(
			glm::mix(minDimensions.x, maxDimensions.x, zoom),
			glm::mix(minDimensions.y, maxDimensions.y, zoom)
		);

	}
}

void OrthoCameraFollow::WindowResize(int width, int height)
{
	cam->viewportBottomLeft = glm::vec2(width - 300 - 20, 20);
	cam->viewportWidthHeight = glm::vec2(300, 200);
}
