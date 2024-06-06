#include "main/GameEngine/ComponentBase/Components/Logic/Player/PlayerController.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

#include "main/GameEngine/Systems/Editor/EditorRuntimeSettings.h"

using namespace std;
using namespace loaders;
using namespace managers;
using namespace component;
using namespace transform;

void PlayerController::Start()
{
	// Get the player body
	playerBody = transform->GetChild(0)->GetChild(0);

	// Add it to the GameManager obervables
	GameManager::GetInstance()->AddResetable(this);

	// Get the initial position of the player
	initialPosition = transform->GetWorldPosition();

	// Set player lives
	lives = maxLives;

	// Add the player skins
	skins.push_back(TextureLoader::Player2);
	skins.push_back(TextureLoader::Player);
	skins.push_back(TextureLoader::Player3);
	ChangeSkin();
}

void PlayerController::Update(const float deltaTime)
{
	// If the game hasn't started, don't move the player but check for difficulty updates
	if (GameManager::GetInstance()->GetGameState() == GameManager::Start) {
		switch (GameManager::GetInstance()->GetGameSpeed()) {
		case GameManager::Snail:
			maxSpeed = 50.0f;
			break;

		case GameManager::Slow:
			maxSpeed = 75.0f;
			break;

		case GameManager::Medium:
			maxSpeed = 100.0f;
			break;

		case GameManager::Fast:
			maxSpeed = 125.0f;
			break;

		case GameManager::Cheetah:
			maxSpeed = 150.0f;
			break;

		case GameManager::LightningMcQueen:
			maxSpeed = 175.0f;
			break;

		default:
			maxSpeed = 100.0f;
		}

		return;
	}

	// Update speed
	speed += acceleration * deltaTime;
	speed = glm::clamp(0.0f, maxSpeed, speed);

	// Otherwise, move the player constantly forward
	transform->Translate(forward * (speed + speed * tilt * tiltFactor) * deltaTime);
}

void PlayerController::GetHit()
{
	lives = glm::clamp(lives - 1, 0, maxLives);

	if (lives > 0) {
		speed = 35.0f;
	}
	else {
		// End the game
		GameManager::GetInstance()->EndGame();
	}
}

void PlayerController::Reset()
{
	// Reset the player position
	transform->SetLocalPosition(initialPosition);

	// Reset the player speed
	speed = 0.0f;

	// Reset player lives
	lives = maxLives;
}

void PlayerController::ChangeSkin()
{
	transform->GetChild(0)->GetChild(0)->GetComponent<MeshRenderer>()->SetTexture(TextureLoader::GetTextureNameFromEnum((TextureLoader::TextureName)skins[skinIndex]));
}

void PlayerController::KeyPress(const int key, const int mods)
{
	if (GameManager::GetInstance()->GetGameState() == GameManager::Start) {
		if (key == GLFW_KEY_RIGHT) {
			skinIndex = (skinIndex + 1) % (int)skins.size();
			ChangeSkin();
		}

		if (key == GLFW_KEY_LEFT) {
			skinIndex = (skinIndex == 0) ? ((int)skins.size() - 1) : (skinIndex - 1);
			ChangeSkin();
		}
	}
}

void PlayerController::MouseMove(const int mouseX, const int mouseY, const int deltaX, const int deltaY)
{
	if (GameManager::GetInstance()->GetGameState() == GameManager::Ended) {
		return;
	}

	// Get the resolution
	glm::vec2 res = EditorRuntimeSettings::resolution;

	// Get the normalized vertical tilt
	const float yRes = res.y;
	tilt = 1.0f * (yRes - mouseY) / yRes;

	// Set the player body tilt
	playerBody->SetLocalRotation(glm::vec3(-tilt * glm::pi<float>() / 6.0f, 0.0f, 0.0f));

	// Translate mouse oX position into rotation
	// [0, resolution.x] pixels -> [-x, x] degrees

	// Get the horizontal resolution
	const float xRes = res.x;
	
	// Move from [0, resolution.x] to [0, 1] and apply a function
	const float normalizedX = (float)mouseX / xRes;
	const float nonlinearX = normalizedX;

	// Move from [0, 1] to [0, 2x] degrees
	const float maxDegrees = 60.0f;
	const float degInterval = nonlinearX * (2.0f * maxDegrees);

	// Move the interval x degrees to the left, to [-x, x]
	const float finalDegrees = degInterval - maxDegrees;

	// Convert degrees to radians
	const float radians = glm::radians(finalDegrees);

	// Rotate the player around the 'up' axis
	glm::mat4 slopeRotation = glm::rotate(glm::mat4(1.0f), radians, defaultUp);

	// Rotate the player onto the ski track
	glm::mat4 slopeAlignment = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6.0f, glm::vec3_right);

	// Multiply them and set the rotation
	transform->SetManualRotationMatrix(slopeRotation * slopeAlignment);

	// Set the new forward vector
	forward = glm::normalize(glm::vec3(
		glm::rotate(glm::mat4(1.0f), radians, defaultUp) * glm::vec4(defaultForward, 1)
	));
}
