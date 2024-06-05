#pragma once
#include <unordered_map>

#include "core/world.h"
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
    void BeginRenderGUI(const World* world);
    void EndRenderGUI();
    void ShutdownGUI();

    bool IsGUIInput();

    void ShowHierarchy(transform::Transform* hierarchy);
    void ShowInspector();

    bool IsGamePlaying() const;
    bool IsGamePaused() const;
    bool IsGameActive() const;

private:
    void ShowMainMenuBar();
    void ShowDemoWindow();

    void ToggleGamePlaying();
    void ToggleGamePaused();
    
    bool showHierarchy = true;
    bool showInspector = true;
    bool showDebugConsole = false;
    bool showDemoWindow = true;

    transform::Transform* lastSelectedTransform = nullptr;

    bool gameIsPlaying = false;
    bool gameIsPaused = false;

    friend class GUIInputListener;
};
