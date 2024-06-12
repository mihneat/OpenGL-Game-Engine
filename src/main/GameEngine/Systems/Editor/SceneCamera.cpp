#include "SceneCamera.h"

#include "main/GameEngine/GUI/GUIManager.h"
#include "main/GameEngine/Managers/InputManager.h"
#include "utils/memory_utils.h"

using namespace component;

void SceneCamera::UpdateValues(glm::vec2 resolution)
{
    // Override WindowResize perspective update of parent
    SetProjection(60.0f, resolution.x / resolution.y);
}

void SceneCamera::InputUpdate(float deltaTime, int mods)
{
    if (!grabbed) return;

    if (mods & GLFW_MOD_SHIFT)
        deltaTime *= superSpeed;
    else
        deltaTime *= speed;

    if (InputManager::KeyHold(GLFW_KEY_W))            TranslateForward(deltaTime);
    if (InputManager::KeyHold(GLFW_KEY_S))            TranslateForward(-deltaTime);
    if (InputManager::KeyHold(GLFW_KEY_A))            TranslateRight(deltaTime);
    if (InputManager::KeyHold(GLFW_KEY_D))            TranslateRight(-deltaTime);
    if (InputManager::KeyHold(GLFW_KEY_E))            TranslateUpward(deltaTime);
    if (InputManager::KeyHold(GLFW_KEY_Q))            TranslateUpward(-deltaTime);
}

void SceneCamera::MouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (!grabbed) return;

    glm::vec2 deltaRads = { glm::radians(rotationSpeed * deltaX), glm::radians(rotationSpeed * deltaY) };

    RotateFirstPerson_OY(-deltaRads.x);
    RotateFirstPerson_OX(deltaRads.y);
}


void SceneCamera::MouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (!GUIManager::GetInstance()->IsSceneHovered()) return;
    
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
    {
        InputManager::ShowCursor(false);
        grabbed = true;
    }
}


void SceneCamera::MouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
    {
        InputManager::ShowCursor(true);
        grabbed = false;
    }
}

void SceneCamera::MouseScroll(const int mouseX, const int mouseY, const int offsetX, const int offsetY)
{
    if (!grabbed) return;

    static constexpr float speedIncreaseAmount = 5.0f;

    speed = glm::max(speed + 1.0f * offsetY * speedIncreaseAmount, 5.0f);
    superSpeed = speed * 2;
    
}

void SceneCamera::FocusTransform(transform::Transform* transform)
{
    // TODO: This doesn't take the object's size into consideration
    this->transform->SetLocalPosition(transform->GetWorldPosition() - this->transform->forward * 25.0f);
}
