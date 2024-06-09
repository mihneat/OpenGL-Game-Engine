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

    bool IsGamePlaying() const;
    bool IsGamePaused() const;
    bool IsGameActive() const;

    bool IsGameWindowResized() const;
    
    bool ReceiveGameInput() const;

    bool ShouldPlay() const;
    bool ShouldPause() const;
    bool ShouldReset() const;
    void UnmarkReset();

    bool IsSceneHovered() const;

    transform::Transform* GetTransformToFocus() const;
    void FinishTransformFocus();

    void ToggleGamePlaying();
    void ToggleGamePaused();

    utils::FBOContainer* GetSceneFBOContainer();
    utils::FBOContainer* GetGameFBOContainer();
    glm::ivec2 GetGameWindowResolution() const;

private:
    void ShowMainMenuBar();
    void ShowMainWindow();
    void ShowDemoWindow();
    void ShowSceneWindow();
    void ShowGameWindow();
    void ShowHierarchy(transform::Transform* hierarchy);
    void ShowInspector();
    void ShowPreferences();

    void LoadPreferences();

    void DisplaySerializedField(const SerializedField& attribute, void* data);
    void DisplaySerializedTransform(transform::Transform* transform);
    
    bool showSceneWindow = true;
    bool showGameWindow = true;
    bool showHierarchy = true;
    bool showInspector = true;
    bool showPreferences = false;
    bool showDebugConsole = false;
    bool showDemoWindow = true;

    transform::Transform* lastSelectedTransform = nullptr;
    
    transform::Transform* transformToFocus = nullptr;

    bool gameIsPlaying = false;
    bool gameIsPaused = false;

    bool isSceneWindowFocused = false;
    bool isGameWindowFocused = false;

    bool isGameWindowResized = false;

    bool isSceneHovered = false;

    bool markStateReset = false;
    bool markStatePlay = false;
    bool markStatePause = false;

    glm::ivec2 gameWindowResolution = {0, 0};

    utils::FBOContainer sceneFBOContainer;
    utils::FBOContainer gameFBOContainer;

    std::unordered_map<std::string, glm::vec4> colors;

    friend class GUIInputListener;
};
