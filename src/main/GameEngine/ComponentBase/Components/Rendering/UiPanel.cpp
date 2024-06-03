#include "main/GameEngine/ComponentBase/Components/Rendering/UiPanel.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

#include <iostream>

using namespace std;
using namespace managers;
using namespace component;
using namespace transform;

void UiPanel::AnchorTop(const float offset)
{
	anchorTop = true;
	offsetTop = offset;
}

void UiPanel::AnchorRight(const float offset)
{
	anchorRight = true;
	offsetRight = offset;
}

void UiPanel::WindowResize(int width, int height)
{
	// Get the resolution
	glm::vec2 res = GameManager::GetInstance()->GetSceneReference()->GetWindow()->GetResolution();

	// Get the new coordinates depending on the anchors
	float oX = transform->GetLocalPosition().x;
	if (anchorRight) {
		oX = res.x - offsetRight;
	}

	float oY = transform->GetLocalPosition().y;
	if (anchorTop) {
		oY = res.y - offsetTop;
	}

	// Set a new scale based on the ratio
	transform->SetLocalPosition(glm::vec3(oX, oY, -200.0f));
}
