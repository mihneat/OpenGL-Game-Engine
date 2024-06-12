#include "GUIInputListener.h"

#include <iostream>
#include <GLFW/glfw3.h>

#include "GUIManager.h"
#include "main/GameEngine/Serialization/CppHeaderParser.h"

void GUIInputListener::OnInputUpdate(float deltaTime, int mods)
{
}

void GUIInputListener::OnKeyPress(int key, int mods)
{
    // --== File ==--
    // Play/Stop game
    if (key == GLFW_KEY_P && mods & GLFW_MOD_CONTROL)
        GUIManager::GetInstance()->markStatePlay = true;
    
    // Pause/Unpause game
    if (key == GLFW_KEY_O && mods & GLFW_MOD_CONTROL)
        GUIManager::GetInstance()->markStatePause = true;

    // Reload scripts
    if (key == GLFW_KEY_R && mods & GLFW_MOD_CONTROL)
        CppHeaderParser::GenerateSerializedData();

    // Save scene
    if (key == GLFW_KEY_S && mods & GLFW_MOD_CONTROL)
        GUIManager::GetInstance()->markStateSave = true;

    // --== Window ==--
    // Show/Hide game window
    if (key == GLFW_KEY_G && mods & GLFW_MOD_CONTROL)
        GUIManager::GetInstance()->showGameWindow = !GUIManager::GetInstance()->showGameWindow;
    
    // Show/Hide hierarchy window
    if (key == GLFW_KEY_H && mods & GLFW_MOD_CONTROL)
        GUIManager::GetInstance()->showHierarchy = !GUIManager::GetInstance()->showHierarchy;
    
    // Show/Hide hierarchy window
    if (key == GLFW_KEY_I && mods & GLFW_MOD_CONTROL)
        GUIManager::GetInstance()->showInspector = !GUIManager::GetInstance()->showInspector;
    
    // Show/Hide ImGui demo window
    if (key == GLFW_KEY_D && mods & GLFW_MOD_CONTROL)
        GUIManager::GetInstance()->showDemoWindow = !GUIManager::GetInstance()->showDemoWindow;

    // --== Scene window ==--
    // Focus currently selected object
    if (key == GLFW_KEY_F && GUIManager::GetInstance()->lastSelectedTransform != nullptr && GUIManager::GetInstance()->IsSceneHovered())
        GUIManager::GetInstance()->transformToFocus = GUIManager::GetInstance()->lastSelectedTransform;
}

void GUIInputListener::OnKeyRelease(int key, int mods)
{
}

void GUIInputListener::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void GUIInputListener::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void GUIInputListener::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void GUIInputListener::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void GUIInputListener::OnWindowResize(int width, int height)
{
}
