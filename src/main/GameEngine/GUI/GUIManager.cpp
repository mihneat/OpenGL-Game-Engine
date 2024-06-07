#include "GUIManager.h"

#include <stack>
#include <unordered_map>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "core/world.h"
#include "main/GameEngine/Managers/TextureLoader.h"
#include "main/GameEngine/Serialization/CppHeaderParser.h"
#include "main/GameEngine/Serialization/Serializer.h"

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
            ImGui::Separator();
            if (ImGui::MenuItem("Reset", "CTRL+R")) { CppHeaderParser::GenerateSerializedData(); }
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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 10.0f));
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
        ImGui::Text("TODO: Here should be the Play / Pause buttons!");
        // ImGui::ImageButton();
        
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
    static int viewType = 0;
    static int fixedResolutionIndex = 0;

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

std::string ExtractClassName(const char* str)
{
    // Make a copy of the string
    std::string fullClassName = std::string(str);
    std::string delimiter = "::";

    const size_t pos = fullClassName.rfind(delimiter);
    std::string token = fullClassName.substr(pos + delimiter.length(), fullClassName.length() - pos);

    return token;
}

bool SerializeBool(bool* data)
{
    if (data == nullptr)
        return false;

    ImGui::Checkbox("##dragBool", data);

    return true;
}

bool SerializeInt(int* data)
{
    if (data == nullptr)
        return false;

    ImGui::DragInt("##dragInt", data, 1);

    return true;
}

bool SerializeFloat(float* data)
{
    if (data == nullptr)
        return false;

    ImGui::DragFloat("##dragFloat", data, 1.0f);

    return true;
}

bool SerializeVec2(glm::vec2* data)
{
    if (data == nullptr)
        return false;

    ImGui::DragFloat2("##dragVec2", reinterpret_cast<float*>(data), 1.0f);

    return true;
}

bool SerializeVec3(glm::vec3* data)
{
    if (data == nullptr)
        return false;

    ImGui::DragFloat3("##dragVec3", reinterpret_cast<float*>(data), 1.0f);

    return true;
}

bool SerializeColour(glm::vec4* data)
{
    if (data == nullptr)
        return false;

    ImGui::DragFloat4("##dragColour", reinterpret_cast<float*>(data), 1.0f);

    return true;
}

bool SerializeTransform(transform::Transform* data)
{
    if (data == nullptr)
        return false;

    ImGui::Text("%s", data->GetName());

    return true;
}

void GUIManager::DisplaySerializedField(const SerializedField& attribute, void* data)
{
    ImGui::AlignTextToFramePadding();
    ImGui::SetNextItemWidth(100);
    ImGui::Text("%s ", attribute.name.c_str());
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

    int successful;
    switch (attribute.type)
    {
    case FieldTypeBool:
        successful = SerializeBool(static_cast<bool*>(data));
        break;
        
    case FieldTypeInt:
        successful = SerializeInt(static_cast<int*>(data));
        break;
        
    case FieldTypeFloat:
        successful = SerializeFloat(static_cast<float*>(data));
        break;
        
    case FieldTypeVec2:
        successful = SerializeVec2(static_cast<glm::vec2*>(data));
        break;
        
    case FieldTypeVec3:
        successful = SerializeVec3(static_cast<glm::vec3*>(data));
        break;
        
    case FieldTypeColour:
        successful = SerializeColour(static_cast<glm::vec4*>(data));
        break;
        
    case FieldTypeTransform:
        successful = SerializeTransform(static_cast<transform::Transform*>(data));
        break;

    default:
        ImGui::Text("Unknown type %d", attribute.type);
        std::cout << "[GUIManager] Type '" << attribute.type << "' not implemented\n";
        return;
    }

    if (!successful)
        std::cerr << "Data serialization failed for attribute " << attribute.name << " of type " << attribute.type << "\n";
}

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

        const std::string className = ExtractClassName(typeid(*curr).name());
        
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        nodeOpen = ImGui::TreeNodeEx(curr, baseObjectNodeFlags, "");
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
