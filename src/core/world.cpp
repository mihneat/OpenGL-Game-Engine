#include "core/world.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "core/engine.h"
#include "components/camera_input.h"
#include "components/transform.h"
#include "main/GameEngine/GUI/GUIManager.h"


World::World()
{
    previousTime = 0;
    elapsedTime = 0;
    deltaTime = 0;
    paused = false;
    shouldClose = false;

    window = Engine::GetWindow();
}


void World::Run()
{
    if (!window)
        return;

    while (true)
    {
        if (window->ShouldClose())
        {
            // TODO: Check for unsaved changes
            // In imgui_demo.cpp, look for the line:
            // if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            break;
        }
        
        LoopUpdate();
    }
}


void World::Pause()
{
    paused = !paused;
}


void World::Exit()
{
    shouldClose = true;
    window->Close();
}


double World::GetLastFrameTime()
{
    return deltaTime;
}


void World::ComputeFrameDeltaTime()
{
    elapsedTime = Engine::GetElapsedTime();
    deltaTime = elapsedTime - previousTime;
    previousTime = elapsedTime;
}


void World::LoopUpdate()
{
    // Polls and buffers the events
    window->PollEvents();

    GUIManager::GetInstance()->BeginRenderGUI(this);

    // Editor frame start processing
    FrameStart();

    // Activate/deactivate game input events
    // const bool activeGameState = IsActive();
    // if (activeGameState != GUIManager::GetInstance()->IsGameActive())
    //     SetActive(GUIManager::GetInstance()->IsGameActive());

    // Computes frame deltaTime in seconds
    ComputeFrameDeltaTime();

    // Update components before input listeners
    PreUpdate();

    // Calls the methods of the instance of InputController in the following order
    // OnWindowResize, OnGameWindowResize, OnMouseMove, OnMouseBtnPress, OnMouseBtnRelease, OnMouseScroll, OnKeyPress, OnMouseScroll, OnInputUpdate
    // OnInputUpdate will be called each frame, the other functions are called only if an event is registered
    window->UpdateObservers();

    // Main loop
    Update(static_cast<float>(deltaTime));
    
    // Editor frame end processing
    FrameEnd();

    GUIManager::GetInstance()->EndRenderGUI();

    // Swap front and back buffers - image will be displayed to the screen
    window->SwapBuffers();
}
