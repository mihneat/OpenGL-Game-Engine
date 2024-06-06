#include <ctime>
#include <iostream>

#include "core/engine.h"
#include "components/simple_scene.h"
#include "main/GameEngine/GUI/GUIInputListener.h"
#include "main/GameEngine/GUI/GUIManager.h"
#include "main/GameEngine/Systems/FileSystem.h"

#if defined(WITH_MAIN)
#   include "main/lab_list.h"
#endif

#if defined(WITH_LAB_M1)
#   include "lab_m1/lab_list.h"
#endif

#if defined(WITH_LAB_M2)
#   include "lab_m2/lab_list.h"
#endif

#if defined(WITH_LAB_EXTRA)
#   include "lab_extra/lab_list.h"
#endif


#ifdef _WIN32
    PREFER_DISCRETE_GPU_NVIDIA;
    PREFER_DISCRETE_GPU_AMD;
#endif


std::string GetParentDir(const std::string &filePath)
{
    size_t pos = filePath.find_last_of("\\/");
    return (std::string::npos == pos) ? "." : filePath.substr(0, pos);
}


int main(int argc, char **argv)
{
    srand((unsigned int)time(NULL));

    // Create a window property structure
    WindowProperties wp;
    wp.resolution = glm::ivec2(1280, 720);
    wp.maximized = true;
    wp.vSync = true;
    wp.selfDir = GetParentDir(std::string(argv[0]));

    // Set the primary file path
    FileSystem::rootDirectory = wp.selfDir;

    // Init the Engine and create a new window with the defined properties
    (void)Engine::Init(wp);

    GUIManager::GetInstance()->InitializeGUI(Engine::GetWindow()->GetHandle());
    GUIInputListener guiInputListener;

    // Create a new 3D world and start running it
    World *world = new m1::GameEngine();

    world->Init();
    world->Run();

    GUIManager::GetInstance()->ShutdownGUI();

    // Signals to the Engine to release the OpenGL context
    Engine::Exit();

    return 0;
}
