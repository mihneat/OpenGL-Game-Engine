#include "SceneManager.h"

using namespace std;
using namespace managers;

std::unordered_map<std::string, transform::Transform *> SceneManager::loadedScenes;

void SceneManager::LoadScene(std::string scenePath, SceneLoadMode mode)
{
    if (mode == Single)
    {
        // Unload all other scenes
        for (const auto& scene : loadedScenes)
        {
            UnloadScene(scene.first);
        }
    } else
    {
        // Check if scene is already loaded, in which case unload it
        if (loadedScenes.find(scenePath) != loadedScenes.end())
        {
            UnloadScene(scenePath);
        }
    }
    
    // TODO: Read scene at given path
    // 
}

void SceneManager::UnloadScene(std::string scenePath)
{
    
}
