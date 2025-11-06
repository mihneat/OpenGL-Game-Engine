#pragma once
#include <unordered_map>

#include "imgui_node_editor.h"
#include "core/world.h"
#include "core/window/window_callbacks.h"
#include "main/GameEngine/ComponentBase/Transform.h"
#include "main/GameEngine/Utils/Containers.h"
#include "ShaderGraph/ShaderGraphManager.h"

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
    bool IsGameStepping() const;
    bool IsGameActive() const;

    bool IsGameWindowResized() const;
    
    bool ReceiveGameInput() const;

    bool ShouldPlay() const;
    bool ShouldPause() const;
    bool ShouldStep() const;
    bool ShouldReset() const;
    void UnmarkReset();
    bool ShouldSave() const;
    void UnmarkSave();
    
    void StopGameStepping();

    bool ShouldReloadShaders() const;
    void MarkReloadShaders();
    void UnmarkReloadShaders();
    
    transform::Transform* GetLastSelectedTransform();

    transform::Transform* RetrieveTransformToCreateChild();
    transform::Transform* RetrieveTransformToDelete();
    
    std::string RetrieveComponentToCreate();
    component::Component* RetrieveComponentToDelete();

    bool IsSceneHovered() const;

    transform::Transform* GetTransformToFocus() const;
    void FinishTransformFocus();

    void ToggleGamePlaying();
    void ToggleGamePaused();
    void ToggleGameStepping();

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
    void ShowShaderGraph();

    void LoadPreferences();

    bool DisplaySerializedField(const SerializedField& attribute, void* data);
    void DisplaySerializedTransform(transform::Transform* transform);

    bool showSceneWindow = true;
    bool showGameWindow = true;
    bool showHierarchy = true;
    bool showInspector = true;
    bool showPreferences = false;
    bool showShaderGraph = false;
    bool showDebugConsole = false;
    bool showDemoWindow = true;

    transform::Transform* lastSelectedTransform = nullptr;
    
    transform::Transform* transformToFocus = nullptr;

    bool gameIsPlaying = false;
    bool gameIsPaused = false;
    bool gameIsStepping = false;

    bool isSceneWindowFocused = false;
    bool isGameWindowFocused = false;

    bool isGameWindowResized = false;

    bool isSceneHovered = false;

    bool markStateReset = false;
    bool markStatePlay = false;
    bool markStatePause = false;
    bool markStateStep = false;
    
    bool markStateSave = false;
    
    bool markReloadShaders = false;

    transform::Transform* transformToCreateChild = nullptr;
    transform::Transform* transformToDelete = nullptr;

    std::string componentToCreate;
    component::Component* componentToDelete = nullptr;

    glm::ivec2 gameWindowResolution = {0, 0};

    utils::FBOContainer sceneFBOContainer;
    utils::FBOContainer gameFBOContainer;

    std::unordered_map<std::string, glm::vec4> colors;

    shader_graph::ShaderGraphManager* shaderGraphManager = new shader_graph::ShaderGraphManager();

    friend class GUIInputListener;
};
