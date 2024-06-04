#pragma once
#include <unordered_map>

#include "core/window/window_callbacks.h"
#include "main/GameEngine/ComponentBase/Transform.h"

class GUIManager
{
public:
    static GUIManager* GetInstance();

private:
    GUIManager() {}

    static GUIManager* instance;

public:
    GUIManager(GUIManager& other) = delete;
    void operator=(const GUIManager&) = delete;

    void InitializeGUI(GLFWwindow* window);
    void BeginRenderGUI();
    void EndRenderGUI();
    void ShutdownGUI();

    bool IsGUIInput();

    void ShowHierarchy(transform::Transform* hierarchy);
    void ShowInspector();

private:
    void ShowMainMenuBar();
    
    bool showHierarchy = true;
    bool showInspector = true;
    bool showDebugConsole = false;

    transform::Transform* lastSelectedTransform = nullptr;
};
