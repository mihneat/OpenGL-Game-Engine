#include "GUIManager.h"

#include <stack>
#include <unordered_map>

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "core/world.h"
#include "main/GameEngine/Managers/TextureLoader.h"
#include "main/GameEngine/Serialization/CppHeaderParser.h"
#include "main/GameEngine/Serialization/Database.h"
#include "main/GameEngine/Serialization/Serializer.h"
#include "main/GameEngine/Systems/Rendering/Texture.h"
#include "main/GameEngine/Utils/Utils.h"

#define BUF_SIZE 64

struct TransformData
{
    transform::Transform* transform;
    int indent;
    int isParentOpen;
};

GUIManager* GUIManager::instance = nullptr;

static const char* MainWindow_PlayBtnActive = "MainWindow_PlayBtnActive";
static const char* MainWindow_PlayBtnInactive = "MainWindow_PlayBtnInactive";
static const char* MainWindow_PauseBtnActive = "MainWindow_PauseBtnActive";
static const char* MainWindow_PauseBtnInactive = "MainWindow_PauseBtnInactive";
static const char* InspectorWindow_Header = "InspectorWindow_Header";

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

    LoadPreferences();
}

void GUIManager::LoadPreferences()
{
    // TODO: Create a editorConfig.ini, read and parse it
    
    colors = {
        {MainWindow_PlayBtnActive, glm::vec4(62, 190, 70, 255) / 255.0f},
        {MainWindow_PlayBtnInactive, glm::vec4(54, 54, 54, 255) / 255.0f},
        {MainWindow_PauseBtnActive, glm::vec4(203, 183, 38, 255) / 255.0f},
        {MainWindow_PauseBtnInactive, glm::vec4(54, 54, 54, 255) / 255.0f},
        {InspectorWindow_Header, glm::vec4(29, 34, 44, 255) / 255.0f}
    };
}

void GUIManager::ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Play", "CTRL+P", this->gameIsPlaying)) { markStatePlay = true; }
            if (ImGui::MenuItem("Pause", "CTRL+O", this->gameIsPaused)) { markStatePause = true; }
            ImGui::Separator();
            if (ImGui::MenuItem("Save scene", "CTRL+S", nullptr, !this->gameIsPlaying)) { markStateSave = true; }
            if (ImGui::MenuItem("Reserialize", "CTRL+R")) { CppHeaderParser::GenerateSerializedData(); }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                ImGui::SetTooltip("Reserialization of scripts requires an editor restart.");
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::MenuItem("Preferences", nullptr, &this->showPreferences);
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
        markStateReset = true;

    gameIsPlaying = !gameIsPlaying;
    markStatePlay = false;
}

void GUIManager::ToggleGamePaused()
{
    gameIsPaused = !gameIsPaused;
    markStatePause = false;
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
    ShowPreferences();
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

void MouseWrap(const char* buf, bool wrapHorizontal, bool wrapVertical, ImGuiMouseCursor_ cursorType)
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        // Check if the active window is the one given as parameter
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (ImGui::GetActiveID() != window->GetID(buf))
            return;

        // Set the mouse cursor type
        ImGui::SetMouseCursor(cursorType);

        // Wrap the mouse around the editor window
        glm::vec2 mousePos = {ImGui::GetMousePos().x, ImGui::GetMousePos().y};
        glm::vec2 windowSize = {ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y};
        constexpr float pad = 5;
        bool teleportMouse = false;
        glm::vec2 newMousePos = mousePos;

        if (wrapHorizontal)
        {
            if (mousePos.x < pad)
            {
                newMousePos.x = windowSize.x - pad;
                teleportMouse = true;
            }

            if (mousePos.x > windowSize.x - pad)
            {
                newMousePos.x = pad;
                teleportMouse = true;
            }
        }

        if (wrapVertical)
        {
            if (mousePos.y < pad)
            {
                newMousePos.y = windowSize.y - pad;
                teleportMouse = true;
            }

            if (mousePos.y > windowSize.y - pad)
            {
                newMousePos.y = pad;
                teleportMouse = true;
            }
        }

        if (teleportMouse)
            ImGui::TeleportMousePos(ImVec2(newMousePos.x, newMousePos.y));
    }
}

ImVec4 GlmVec4ToImVec4(glm::vec4 col, float add = 0.0f)
{
    return {col.r + add, col.g + add, col.b + add, col.a};
}

void GUIManager::ShowMainWindow()
{
    static ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus;

    // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
    // Based on your use case you may want one or the other.
    constexpr ImVec2 framePadding = ImVec2(0.0f, 10.0f);
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, framePadding);
    if (!ImGui::Begin("Main", nullptr, flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::PopStyleVar(2);
        ImGui::End();
        return;
    }
    
    ImGui::PopStyleVar(2);

    // Create the dockspace
    constexpr ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    const ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

    // Create a menu bar that displays the resolution
    if (ImGui::BeginMenuBar())
    {
        // Center buttons horizontally
        constexpr ImVec2 buttonSize = ImVec2(70.0f, 25.0f);
        ImGuiStyle& style = ImGui::GetStyle();
        const float contentSize = buttonSize.x * 2.0f + style.ItemSpacing.x + framePadding.x * 2.0f;
        const float available = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((available - contentSize) / 2.0f);

        // Center buttons vertically
        const float menuBarHeight = ImGui::GetFrameHeight();
        ImGui::SetCursorPosY(menuBarHeight - buttonSize.y + framePadding.y);

        // Retrieve button colors
        glm::vec4 playBtnCol = IsGamePlaying() ? colors[MainWindow_PlayBtnActive] : colors[MainWindow_PlayBtnInactive];
        glm::vec4 pauseBtnCol = IsGamePaused() ? colors[MainWindow_PauseBtnActive] : colors[MainWindow_PauseBtnInactive];
        
        ImGui::PushStyleColor(ImGuiCol_Button, GlmVec4ToImVec4(playBtnCol));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, GlmVec4ToImVec4(playBtnCol, 0.05f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, GlmVec4ToImVec4(playBtnCol,  0.15f));
        if (ImGui::Button("Play##PlayBtn", buttonSize)) markStatePlay = true;
        ImGui::PopStyleColor(3);
        
        ImGui::PushStyleColor(ImGuiCol_Button, GlmVec4ToImVec4(pauseBtnCol));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, GlmVec4ToImVec4(pauseBtnCol, 0.05f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, GlmVec4ToImVec4(pauseBtnCol, 0.15f));
        if (ImGui::Button("Pause##PauseBtn", buttonSize)) markStatePause = true;
        ImGui::PopStyleColor(3);
        
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

    // Check if the scene window is focused and hovered
    isSceneWindowFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    isSceneHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

    // Get the scene window size
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
    ImGui::Image((ImTextureID)sceneFBOContainer.GetColorTextureID(), imgSize, {0, 1}, {1, 0});
    
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
    const glm::ivec2 windowResolution = {
        floor(glm::max(imgSize.x, minResolution.x)),
        floor(glm::max(imgSize.y, minResolution.y))
    };

    static const char* resolutionTexts[] = {
        "800 x 600",
        "1024 x 768",
        "1280 x 720",
        "1600 x 900",
        "1920 x 1080",
    };
    static constexpr glm::ivec2 resolutions[] = {
        glm::ivec2(800, 600),   // 4:3
        glm::ivec2(1024, 768),  // 4:3
        glm::ivec2(1280, 720),  // 16:9
        glm::ivec2(1600, 900),  // 16:9 
        glm::ivec2(1920, 1080), // 16:9
    };
    static const char* items[] = { "Free", "Fixed" };
    static int viewType = 1;
    static int fixedResolutionIndex = 2;

    // Create a menu bar that displays the resolution
    if (ImGui::BeginMenuBar())
    {
        ImGui::Text("View:");
        
        static ImGuiComboFlags comboFlags = ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_NoArrowButton;
        if (ImGui::BeginCombo(" ##Game View Dropdown", items[viewType], comboFlags))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool isSelected = (viewType == n);
                if (ImGui::Selectable(items[n], isSelected))
                    viewType = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (viewType == 0)
        {
            // Free View
            ImGui::Text("Resolution: %d x %d", windowResolution.x, windowResolution.y);
        } else
        {
            // Fixed View
            ImGui::Text("Resolution:");
            if (ImGui::BeginCombo(" ##Resolution Dropdown", resolutionTexts[fixedResolutionIndex], comboFlags))
            {
                for (int n = 0; n < IM_ARRAYSIZE(resolutions); n++)
                {
                    const bool isSelected = (fixedResolutionIndex == n);
                    if (ImGui::Selectable(resolutionTexts[n], isSelected))
                        fixedResolutionIndex = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        
        ImGui::EndMenuBar();
    }

    // Set resolution depending on view type
    const glm::ivec2 gameResolution = viewType == 0 ? windowResolution : resolutions[fixedResolutionIndex];

    // Set the new FBO resolution (no-op if same)
    gameFBOContainer.SetResolution(gameResolution);

    // Check for window resize
    isGameWindowResized = gameWindowResolution != gameResolution;
    gameWindowResolution = gameResolution;

    // Compute the texture size
    glm::vec2 offset(0.0f, 0.0f);
    ImVec2 finalImgSize = imgSize;
    if (viewType == 1)
    {
        // Try fit width
        const float widthFactor = (float)gameResolution.x / (float)windowResolution.x;
        if ((float)gameResolution.y / widthFactor <= (float)windowResolution.y)
        {
            // Fit width
            finalImgSize = ImVec2(
                (float)gameResolution.x / widthFactor,
                (float)gameResolution.y / widthFactor
            );

            // Formula: Remaining height / 2.0f
            offset.y = ((float)windowResolution.y - finalImgSize.y) / 2.0f;
        }
        else
        {
            // Fit height
            const float heightFactor = (float)gameResolution.y / (float)windowResolution.y;
            finalImgSize = ImVec2(
                (float)gameResolution.x / heightFactor,
                (float)gameResolution.y / heightFactor
            );

            // Formula: Remaining width / 2.0f
            offset.x = ((float)windowResolution.x - finalImgSize.x) / 2.0f;
        }
    }
    
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset.x);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset.y);

    // Render the game texture into the window, also flip it vertically
    ImGui::Image((ImTextureID)gameFBOContainer.GetColorTextureID(), finalImgSize, {0, 1}, {1, 0});
    
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
    if (!ImGui::Begin("Hierarchy", &this->showHierarchy, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("New.."))
        {
            if (ImGui::MenuItem("Transform"))
                transformToCreateChild = hierarchy;
                
            ImGui::EndMenu();
        }
            
        ImGui::EndMenuBar();
    }

    ImGuiTreeNodeFlags baseObjectNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    // Make a stack of pairs containing the transform and the indent level
    std::stack<TransformData> transformStack;

    // Add all the children to the stack
    for (int i = hierarchy->GetChildCount() - 1; i >= 0; --i) {
        transformStack.push({hierarchy->GetChild(i), 0, true});
    }

    int prevIndent = -1;
    bool selectedTransformStillExists = false;

    // Go through all transforms in the scene
    while (!transformStack.empty()) {
        // Update the root
        const TransformData currentTransform = transformStack.top();
        transformStack.pop();

        for (int i = currentTransform.indent; i < prevIndent; ++i)
            ImGui::TreePop();

        if (currentTransform.isParentOpen)
            prevIndent = currentTransform.indent;

        // Render the object only if the parent is open
        bool nodeOpen = false;
        if (currentTransform.isParentOpen)
        {
            ImGuiTreeNodeFlags objectNodeFlags = baseObjectNodeFlags;
            if (lastSelectedTransform == currentTransform.transform)
                objectNodeFlags |= ImGuiTreeNodeFlags_Selected;
            if (currentTransform.transform->GetChildCount() == 0)
                objectNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        
            nodeOpen = ImGui::TreeNodeEx(currentTransform.transform, objectNodeFlags, "");
            
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                lastSelectedTransform = currentTransform.transform;

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::Selectable("Add New Transform"))
                    transformToCreateChild = currentTransform.transform;
                if (ImGui::Selectable("Delete"))
                    transformToDelete = currentTransform.transform;
                
                ImGui::EndPopup();
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
                currentTransform.indent + 1, currentTransform.isParentOpen && nodeOpen});
        }
    }

    for (int i = 0; i < prevIndent; ++i)
        ImGui::TreePop();

    // Possible issue: the inspector is updated AFTER destroy but BEFORE new hierarchy
    // Might lead to an error/memory leak further down the road but I hope not!!
    if (!selectedTransformStillExists)
        lastSelectedTransform = nullptr;
    
    ImGui::End();
}

bool SerializeBool(bool* data)
{
    if (data == nullptr)
        return false;

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##SerializeBool %p", (void*)data);

    bool changed = false;
    
    if (ImGui::Checkbox(buf, data)) changed = true;

    return changed;
}

bool SerializeInt(int* data)
{
    if (data == nullptr)
        return false;

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##SerializeInt %p", (void*)data);

    bool changed = false;
    
    if (ImGui::DragInt(buf, data, 1)) changed = true;
    
    MouseWrap(buf, true, false, ImGuiMouseCursor_ResizeEW);

    return changed;
}

bool SerializeFloat(float* data)
{
    if (data == nullptr)
        return false;

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##SerializeFloat %p", (void*)data);

    bool changed = false;
    
    if (ImGui::DragFloat(buf, data, 0.02f)) changed = true;
        
    MouseWrap(buf, true, false, ImGuiMouseCursor_ResizeEW);

    return changed;
}

bool SerializeVec2(glm::vec2* data)
{
    if (data == nullptr)
        return false;

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##SerializeVec2 %p", (void*)data);

    bool changed = false;
    
    if (ImGui::DragFloat2(buf, reinterpret_cast<float*>(data), 0.02f)) changed = true;

    // Check ImGui::DragScalarN() for more information
    ImGui::PushID(buf);
    for (int i = 0; i < 2; ++i)
    {
        ImGui::PushID(i);
        MouseWrap("", true, false, ImGuiMouseCursor_ResizeEW);
        ImGui::PopID();
    }
    
    ImGui::PopID();

    return changed;
}

bool SerializeVec3(glm::vec3* data)
{
    if (data == nullptr)
        return false;

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##SerializeVec3 %p", (void*)data);

    bool changed = false;
    
    if (ImGui::DragFloat3(buf, reinterpret_cast<float*>(data), 0.02f)) changed = true;

    // Check ImGui::DragScalarN() for more information
    ImGui::PushID(buf);
    for (int i = 0; i < 3; ++i)
    {
        ImGui::PushID(i);
        MouseWrap("", true, false, ImGuiMouseCursor_ResizeEW);
        ImGui::PopID();
    }
    
    ImGui::PopID();

    return changed;
}

bool SerializeColour(glm::vec4* data)
{
    if (data == nullptr)
        return false;

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##SerializeColour %p", (void*)data);

    bool changed = false;
    
    if (ImGui::ColorEdit4(buf, reinterpret_cast<float*>(data))) changed = true;

    // Check ImGui::ColorEdit4() for more information
    ImGui::PushID(buf);
    ImGui::PushID("##X");
    MouseWrap("", true, false, ImGuiMouseCursor_ResizeEW);
    ImGui::PopID();
    ImGui::PushID("##Y");
    MouseWrap("", true, false, ImGuiMouseCursor_ResizeEW);
    ImGui::PopID();
    ImGui::PushID("##Z");
    MouseWrap("", true, false, ImGuiMouseCursor_ResizeEW);
    ImGui::PopID();
    ImGui::PushID("##W");
    MouseWrap("", true, false, ImGuiMouseCursor_ResizeEW);
    ImGui::PopID();
    ImGui::PopID();

    return changed;
}

bool SerializeString(std::string* data)
{
    if (data == nullptr)
        return false;

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##SerializeString %p", (void*)data);

    bool changed = false;
    
    if (ImGui::InputText(buf, data)) changed = true;

    return changed;
}

bool SerializeEnum(int* data, const std::string& enumName)
{
    if (data == nullptr)
        return false;

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##SerializeEnum %p", (void*)data);

    int value = *data;
    auto& values = Serializer::GetValuePairsForEnum(enumName);

    if (value >= values.size())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
        ImGui::Text("Enum value out of range");
        ImGui::PopStyleColor(1);

        return false;
    }

    bool changed = false;
    
    static ImGuiComboFlags comboFlags = ImGuiComboFlags_WidthFitPreview;
    if (ImGui::BeginCombo(buf, values[value].first.c_str(), comboFlags))
    {
        for (int n = 0; n < values.size(); n++)
        {
            bool isSelected = (value == n);
            if (ImGui::Selectable(values[n].first.c_str(), isSelected))
            {
                isSelected = n;
                *data = n;
                changed = true;
            }
    
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    return changed;
}

bool SerializeTransform(transform::Transform** data)
{
    if (data == nullptr)
        return false;

    ImGui::Text("%s", *data == nullptr ? "NULL" : (*data)->GetName().c_str());

    return false;
}

bool SerializeGUID(void* data, const std::string& typeName)
{
    if (data == nullptr)
        return false;

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##SerializeGUID %p", data);
    
    SerializableObject** dataObj = static_cast<SerializableObject**>(data);

    if (dataObj == nullptr || typeName.empty())
    {
        ImGui::Text("Unknown type");
        return false;
    }
        
    SerializedObject obj = Database::GetByPtr(*dataObj);

    const std::map<std::string, SerializedObject>& objs = Database::GetByType(typeName);

    // Transform into a vector
    int value = -1, i = 0;
    std::vector<SerializedObject> values;
    for (auto& it : objs)
    {
        if (obj.ptr == it.second.ptr)
            value = i;
            
        values.push_back(it.second);

        ++i;
    }

    int initValue = value;
    
    static ImGuiComboFlags comboFlags = ImGuiComboFlags_WidthFitPreview;
    if (ImGui::BeginCombo(buf, value == -1 ? "None" : values[value].name.c_str(), comboFlags))
    {
        bool isSelected = (value == -1);
        if (ImGui::Selectable("None", isSelected))
            value = -1;
        
        if (isSelected)
            ImGui::SetItemDefaultFocus();
        
        for (int n = 0; n < values.size(); n++)
        {
            isSelected = (value == n);
            if (ImGui::Selectable(values[n].name.c_str(), isSelected))
            {
                value = n;
            }
    
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
                
    // Assign the new data
    *dataObj = value == -1 ? nullptr : static_cast<SerializableObject*>(values[value].ptr);

    return value == initValue;
}

std::string FormatString(const std::string& str)
{
    std::string formattedString(str);
    formattedString[0] = toupper(formattedString[0]);

    // Insert a space before each capital letter
    for (size_t i = 1; i < formattedString.length(); ++i)
    {
        if (formattedString[i] >= 'A' && formattedString[i] <= 'Z') {
            formattedString.insert(i, " ");
            
            // Skip over the character
            ++i;
        }
    }

    return formattedString;
}

bool GUIManager::DisplaySerializedField(const SerializedField& attribute, void* data)
{
    ImGui::AlignTextToFramePadding();
    ImGui::SetNextItemWidth(100);
    ImGui::Text("%s ", FormatString(attribute.name).c_str());
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    bool changed = false;
    switch (attribute.type)
    {
    case FieldTypeBool:
        changed = SerializeBool(static_cast<bool*>(data));
        break;
        
    case FieldTypeInt:
        changed = SerializeInt(static_cast<int*>(data));
        break;
        
    case FieldTypeFloat:
        changed = SerializeFloat(static_cast<float*>(data));
        break;
        
    case FieldTypeVec2:
        changed = SerializeVec2(static_cast<glm::vec2*>(data));
        break;
        
    case FieldTypeVec3:
        changed = SerializeVec3(static_cast<glm::vec3*>(data));
        break;
        
    case FieldTypeColour:
        changed = SerializeColour(static_cast<glm::vec4*>(data));
        break;
        
    case FieldTypeString:
        changed = SerializeString(static_cast<std::string*>(data));
        break;

    case FieldTypeEnum:
        changed = SerializeEnum(static_cast<int*>(data), attribute.typeName);
        break;
        
    case FieldTypeTransform:
        changed = SerializeTransform(static_cast<transform::Transform**>(data));
        break;
        
    case FieldTypeGUID:
        changed = SerializeGUID(data, attribute.typeName);
        break;

    default:
        ImGui::Text("Unknown type %d", attribute.type);
        std::cout << "[GUIManager] Type '" << attribute.type << "' not implemented\n";
        return false;
    }

    return changed;

    // if (!changed)
    //     std::cerr << "Data serialization failed for attribute " << attribute.name << " of type " << attribute.type << "\n";
}

void GUIManager::DisplaySerializedTransform(transform::Transform* transform)
{
    DisplaySerializedField({"name", FieldTypeString}, &transform->name);
    DisplaySerializedField({"tag", FieldTypeString}, &transform->tag);
    const bool positionChanged = DisplaySerializedField({"position", FieldTypeVec3}, &transform->localPosition);
    const bool rotationChanged = DisplaySerializedField({"rotation", FieldTypeVec3}, &transform->localRotation);
    const bool scaleChanged = DisplaySerializedField({"scale", FieldTypeVec3}, &transform->localScale);

    if (rotationChanged) transform->ComputeDirectionVectors();
    if (positionChanged || scaleChanged) transform->ComputeModelMatrix();
}

void GUIManager::ShowInspector()
{
    if (!this->showInspector)
        return;
    
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Inspector", &this->showInspector, ImGuiWindowFlags_MenuBar))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    
    if (ImGui::BeginMenuBar())
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(FLT_MAX, 350));
        if (ImGui::BeginMenu("Add Component..", this->lastSelectedTransform != nullptr))
        {
            static ImGuiTextFilter filter;
            filter.Draw();
            const std::vector<std::string>& classNames = Serializer::GetSerializedClasses(); 
            for (int i = 0; i < classNames.size(); i++)
                if (filter.PassFilter(classNames[i].c_str()))
                {
                    if (ImGui::Selectable(classNames[i].c_str()))
                    {
                        componentToCreate = classNames[i];
                        filter.Clear();
                    }
                }
                
            ImGui::EndMenu();
        }
            
        ImGui::EndMenuBar();
    }

    if (this->lastSelectedTransform == nullptr)
    {
        ImGui::End();
        return;
    }
    
    ImGuiTreeNodeFlags baseObjectNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;
    
    ImGui::PushStyleColor(ImGuiCol_Header, GlmVec4ToImVec4(colors[InspectorWindow_Header]));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, GlmVec4ToImVec4(colors[InspectorWindow_Header], 0.05f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, GlmVec4ToImVec4(colors[InspectorWindow_Header], 0.15f));
    
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    bool nodeOpen = ImGui::TreeNodeEx(lastSelectedTransform, baseObjectNodeFlags, "");
    ImGui::SameLine();
    // ImGui::Checkbox(lastSelectedTransform, &lastSelectedTransform->);
    // ImGui::SameLine();
    ImGui::Text("Transform");

    if (nodeOpen)
    {
        DisplaySerializedTransform(lastSelectedTransform);
        ImGui::TreePop();
    }

    for (int i = 0; i < this->lastSelectedTransform->GetComponentCount(); ++i) {
        component::Component* curr = this->lastSelectedTransform->GetComponentByIndex(i);

        const std::string className = Utils::ExtractClassName(typeid(*curr).name());
        
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        nodeOpen = ImGui::TreeNodeEx(curr, baseObjectNodeFlags, "");

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::Selectable("Delete"))
                componentToDelete = curr;
                
            ImGui::EndPopup();
        }
        
        ImGui::SameLine();
        ImGui::PushID(curr); ImGui::Checkbox("", &curr->enabled); ImGui::PopID();
        ImGui::SameLine();
        ImGui::Text("%s", className.c_str());

        if (!nodeOpen)
            continue;

        // std::cout << "Serializing class '" << className << "'\n";

        for (const auto& attribute : Serializer::GetSerializedFieldsForClass(className))
        {
            // std::cout << "Serializing attribute '" << attribute.name << "' of type '" << attribute.type << "'\n";
            void* data = Serializer::GetAttributeReference(curr, attribute.name);

            DisplaySerializedField(attribute, data);
        }
        
        ImGui::TreePop();
    }
    
    ImGui::PopStyleColor(3);
    
    ImGui::End();
}

void GUIManager::ShowPreferences()
{
    if (!this->showPreferences)
        return;
    
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Preferences", &this->showPreferences))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }
    
    ImGui::Text("Play button active"); ImGui::SameLine();
    ImGui::ColorEdit4("##Play btn active color", reinterpret_cast<float*>(&colors[MainWindow_PlayBtnActive]));
    ImGui::Text("Play button inactive"); ImGui::SameLine();
    ImGui::ColorEdit4("##Play button inactive color", reinterpret_cast<float*>(&colors[MainWindow_PlayBtnInactive]));
    
    ImGui::Text("Pause button active"); ImGui::SameLine();
    ImGui::ColorEdit4("##Pause btn active color", reinterpret_cast<float*>(&colors[MainWindow_PauseBtnActive]));
    ImGui::Text("Pause button inactive"); ImGui::SameLine();
    ImGui::ColorEdit4("##Pause button inactive color", reinterpret_cast<float*>(&colors[MainWindow_PauseBtnInactive]));
    
    ImGui::Text("Inspector header"); ImGui::SameLine();
    ImGui::ColorEdit4("##Inspector header color", reinterpret_cast<float*>(&colors[InspectorWindow_Header]));
    
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

bool GUIManager::ShouldPlay() const
{
    return markStatePlay;
}

bool GUIManager::ShouldPause() const
{
    return markStatePause;
}

bool GUIManager::ShouldReset() const
{
    return markStateReset;
}

void GUIManager::UnmarkReset()
{
    markStateReset = false;
}

bool GUIManager::ShouldSave() const
{
    return markStateSave;
}

void GUIManager::UnmarkSave()
{
    markStateSave = false;
}

transform::Transform* GUIManager::GetLastSelectedTransform()
{
    return lastSelectedTransform;
}

transform::Transform* GUIManager::RetrieveTransformToCreateChild()
{
    transform::Transform* tmp = transformToCreateChild;
    transformToCreateChild = nullptr;
    
    return tmp;
}

transform::Transform* GUIManager::RetrieveTransformToDelete()
{
    transform::Transform* tmp = transformToDelete;
    transformToDelete = nullptr;
    
    return tmp;
}

std::string GUIManager::RetrieveComponentToCreate()
{
    std::string tmp = componentToCreate;
    componentToCreate = "";
    
    return tmp;
}

component::Component* GUIManager::RetrieveComponentToDelete()
{
    component::Component* tmp = componentToDelete;
    componentToDelete = nullptr;
    
    return tmp;
}

bool GUIManager::IsSceneHovered() const
{
    return isSceneHovered;
}

transform::Transform* GUIManager::GetTransformToFocus() const
{
    return transformToFocus;
}

void GUIManager::FinishTransformFocus()
{
    transformToFocus = nullptr;
}

utils::FBOContainer* GUIManager::GetSceneFBOContainer()
{
    return &sceneFBOContainer;
}

utils::FBOContainer* GUIManager::GetGameFBOContainer()
{
    return &gameFBOContainer;
}

glm::ivec2 GUIManager::GetGameWindowResolution() const
{
    return gameFBOContainer.GetResolution();
}
