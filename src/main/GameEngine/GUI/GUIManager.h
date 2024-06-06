#pragma once
#include <unordered_map>

#include "imgui.h"
#include "core/world.h"
#include "core/window/window_callbacks.h"
#include "main/GameEngine/ComponentBase/Transform.h"
#include "main/GameEngine/Utils/Containers.h"

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

    void ShowSceneWindow();
    void ShowGameWindow();
    void ShowHierarchy(transform::Transform* hierarchy);
    void ShowInspector();

    bool IsGamePlaying() const;
    bool IsGamePaused() const;
    bool IsGameActive() const;

    bool IsGameWindowResized() const;
    
    bool ReceiveGameInput() const;

    utils::FBOContainer* GetGameFBOContainer();
    glm::ivec2 GetGameWindowResolution() const;

private:
    void ShowMainMenuBar();
    void ShowMainWindow();
    void ShowDemoWindow();

    void ToggleGamePlaying();
    void ToggleGamePaused();
    
    bool showSceneWindow = true;
    bool showGameWindow = true;
    bool showHierarchy = true;
    bool showInspector = true;
    bool showDebugConsole = false;
    bool showDemoWindow = true;

    transform::Transform* lastSelectedTransform = nullptr;

    bool gameIsPlaying = false;
    bool gameIsPaused = false;

    bool isSceneWindowFocused = false;
    bool isGameWindowFocused = false;

    bool isGameWindowResized = false;

    glm::ivec2 gameWindowResolution = {0, 0};

    utils::FBOContainer sceneFBOContainer;
    utils::FBOContainer gameFBOContainer;

    friend class GUIInputListener;
};
