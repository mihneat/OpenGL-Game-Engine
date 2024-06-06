#include "GUIManager.h"

#include <stack>
#include <unordered_map>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "core/world.h"
#include "main/GameEngine/Managers/TextureLoader.h"

struct transform_data
{
    transform::Transform* transform;
    int indent;
    int is_parent_open;
};

GUIManager* GUIManager::instance = nullptr;

GUIManager* GUIManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new GUIManager();
    }
    
    return instance;
}

void GUIManager::InitializeGUI(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void GUIManager::ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Play", "CTRL+P", this->gameIsPlaying)) { ToggleGamePlaying(); }
            if (ImGui::MenuItem("Pause", "CTRL+O", this->gameIsPaused)) { ToggleGamePaused(); }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Window"))
        {
            ImGui::MenuItem("Save layout");
            ImGui::MenuItem("Load layout");
            ImGui::Separator();
            ImGui::MenuItem("Scene", nullptr, &this->showSceneWindow);
            ImGui::MenuItem("Game", "CTRL+G", &this->showGameWindow);
            ImGui::MenuItem("Hierarchy", "CTRL+H", &this->showHierarchy);
            ImGui::MenuItem("Inspector", "CTRL+I", &this->showInspector);
            ImGui::MenuItem("Debug console", nullptr, &this->showDebugConsole);
            ImGui::MenuItem("ImGui Demo <3", "CTRL+D", &this->showDemoWindow);
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void GUIManager::ToggleGamePlaying()
{
    // Stop the game
    if (gameIsPlaying)
    {
        // TODO: Reset all object values
    }

    gameIsPlaying = !gameIsPlaying;
}

void GUIManager::ToggleGamePaused()
{
    gameIsPaused = !gameIsPaused;
}

void GUIManager::BeginRenderGUI(const World* world)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ShowMainMenuBar();
    ShowMainWindow();
    ShowSceneWindow();
    ShowGameWindow();
    ShowHierarchy(world->hierarchy);
    ShowInspector();
    ShowDemoWindow();
}

void GUIManager::EndRenderGUI()
{
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIManager::ShutdownGUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool GUIManager::IsGUIInput()
{
    return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

void GUIManager::ShowMainWindow()
{
    static ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus;

    // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
    // Based on your use case you may want one or the other.
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    if (!ImGui::Begin("Main", nullptr, flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }
    
    ImGui::PopStyleVar();

    // Create the dockspace
    constexpr ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    const ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

    // Create a menu bar that displays the resolution
    if (ImGui::BeginMenuBar())
    {
        ImGui::Text("TODO: Here should be the Play / Pause buttons!");
        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void GUIManager::ShowSceneWindow()
{
    if (!this->showSceneWindow)
        return;

    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
    
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (!ImGui::Begin("Scene", &this->showSceneWindow, flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }
    
    ImGui::PopStyleVar();

    // Check if the game window is focused
    isSceneWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    // Get the game window size
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const ImVec2 imgSize(vMax.x - vMin.x, vMax.y - vMin.y);

    // Compute and clamp the resolution
    constexpr glm::vec2 minResolution = {10.0f, 10.0f};
    const glm::ivec2 resolution = {
        floor(glm::max(imgSize.x, minResolution.x)),
        floor(glm::max(imgSize.y, minResolution.y))
    };

    // Set the new FBO resolution (no-op if same)
    sceneFBOContainer.SetResolution(resolution);

    // Create a menu bar that displays the resolution
    if (ImGui::BeginMenuBar())
    {
        ImGui::Text("Scene Resolution: %d x %d", resolution.x, resolution.y);
        ImGui::EndMenuBar();
    }

    // Render the game texture into the window, also flip it vertically
    ImGui::Image((ImTextureID)sceneFBOContainer.GetColorTextureID(), imgSize, {0, 0}, {1, 1});
    
    ImGui::End();
}

void GUIManager::ShowGameWindow()
{
    if (!this->showGameWindow)
        return;

    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
    
    ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (!ImGui::Begin("Game", &this->showGameWindow, flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::PopStyleVar();
        ImGui::End();
        return;
    }
    
    ImGui::PopStyleVar();

    // Check if the game window is focused
    isGameWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    // Get the game window size
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const ImVec2 imgSize(vMax.x - vMin.x, vMax.y - vMin.y);

    // Compute and clamp the resolution
    constexpr glm::vec2 minResolution = {10.0f, 10.0f};
    const glm::ivec2 resolution = {
        floor(glm::max(imgSize.x, minResolution.x)),
        floor(glm::max(imgSize.y, minResolution.y))
    };

    // Set the new FBO resolution (no-op if same)
    gameFBOContainer.SetResolution(resolution);

    // Check for window resize
    isGameWindowResized = gameWindowResolution != resolution;
    gameWindowResolution = resolution;

    // Create a menu bar that displays the resolution
    if (ImGui::BeginMenuBar())
    {
        ImGui::Text("Resolution: %d x %d", resolution.x, resolution.y);
        ImGui::EndMenuBar();
    }

    // Render the game texture into the window, also flip it vertically
    ImGui::Image((ImTextureID)gameFBOContainer.GetColorTextureID(), imgSize, {0, 1}, {1, 0});
    
    ImGui::End();
}

void GUIManager::ShowDemoWindow()
{
    if (!this->showDemoWindow)
        return;
    
    ImGui::ShowDemoWindow(&this->showDemoWindow); // Show demo window! :)
}

void GUIManager::ShowHierarchy(transform::Transform* hierarchy)
{
    if (!this->showHierarchy)
        return;
    
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Hierarchy", &this->showHierarchy))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    ImGuiTreeNodeFlags baseObjectNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    // Make a stack of pairs containing the transform and the indent level
    std::stack<transform_data> transformStack;

    // Add all the children to the stack
    for (int i = hierarchy->GetChildCount() - 1; i >= 0; --i) {
        transformStack.push({hierarchy->GetChild(i), 0, true});
    }

    int prevIndent = -1;
    bool selectedTransformStillExists = false;

    // Go through all transforms in the scene
    while (!transformStack.empty()) {
        // Update the root
        const transform_data currentTransform = transformStack.top();
        transformStack.pop();

        for (int i = currentTransform.indent; i < prevIndent; ++i)
            ImGui::TreePop();

        if (currentTransform.is_parent_open)
            prevIndent = currentTransform.indent;

        // Render the object only if the parent is open
        bool nodeOpen = false;
        if (currentTransform.is_parent_open)
        {
            ImGuiTreeNodeFlags objectNodeFlags = baseObjectNodeFlags;
            if (lastSelectedTransform == currentTransform.transform)
                objectNodeFlags |= ImGuiTreeNodeFlags_Selected;
            if (currentTransform.transform->GetChildCount() == 0)
                objectNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        
            nodeOpen = ImGui::TreeNodeEx(currentTransform.transform, objectNodeFlags, ""); // "%s", buf);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                lastSelectedTransform = currentTransform.transform;
                std::cout << "Selected transform: " << currentTransform.transform->GetName() << "\n";
            }
        
            ImGui::SameLine();
        
            ImGui::Text("%s", currentTransform.transform->GetName().c_str());
        }

        // Check if the current transform is the selected one
        if (currentTransform.transform == lastSelectedTransform)
            selectedTransformStillExists = true;

        // Add all the children to the stack
        for (int i = currentTransform.transform->GetChildCount() - 1; i >= 0; --i) {
            transformStack.push({currentTransform.transform->GetChild(i),
                currentTransform.indent + 1, currentTransform.is_parent_open && nodeOpen});
        }
    }

    // Possible issue: the inspector is updated AFTER destroy but BEFORE new hierarchy
    // Might lead to an error/memory leak further down the road but I hope not!!
    if (!selectedTransformStillExists)
        lastSelectedTransform = nullptr;
    
    ImGui::End();
}

// void ExtractClassName(const char* str)
// {
//     std::string s = std::string(str);
//     std::string delimiter = "::";
//
//     size_t pos = 0;
//     std::string token;
//     while ((pos = s.find(delimiter)) != std::string::npos) {
//         token = s.substr(0, pos);
//         std::cout << token << std::endl;
//         s.erase(0, pos + delimiter.length());
//     }
//     std::cout << s << std::endl;
// }

void GUIManager::ShowInspector()
{
    if (!this->showInspector)
        return;
    
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Inspector", &this->showInspector))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    if (this->lastSelectedTransform == nullptr)
    {
        ImGui::End();
        return;
    }
    
    ImGuiTreeNodeFlags baseObjectNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    bool nodeOpen = ImGui::TreeNodeEx(lastSelectedTransform, baseObjectNodeFlags, "");
    ImGui::SameLine();
    ImGui::Text("Transform");

    if (nodeOpen)
    {
        ImGui::Text("Name: %s", lastSelectedTransform->GetName().c_str());
        ImGui::TreePop();
    }

    for (int i = 0; i < this->lastSelectedTransform->GetComponentCount(); ++i) {
        component::Component* curr = this->lastSelectedTransform->GetComponentByIndex(i);

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        nodeOpen = ImGui::TreeNodeEx(curr, baseObjectNodeFlags, "");
        ImGui::SameLine();
        ImGui::Text("%s", typeid(*curr).name());

        if (nodeOpen)
        {
            ImGui::TreePop();
        }
    }
    
    ImGui::End();
}

bool GUIManager::IsGamePlaying() const
{
    return gameIsPlaying;
}

bool GUIManager::IsGamePaused() const
{
    return gameIsPaused;
}

bool GUIManager::IsGameActive() const
{
    return gameIsPlaying && !gameIsPaused;
}

bool GUIManager::IsGameWindowResized() const
{
    return isGameWindowResized;
}

bool GUIManager::ReceiveGameInput() const
{
    return isGameWindowFocused && IsGameActive();
}

utils::FBOContainer* GUIManager::GetGameFBOContainer()
{
    return &gameFBOContainer;
}

glm::ivec2 GUIManager::GetGameWindowResolution() const
{
    return gameFBOContainer.GetResolution();
}
