#include "SceneManager.h"

#include <fstream>

#include "nlohmann/json.hpp"

using namespace std;
using namespace component;
using json = nlohmann::json;

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
    
    // Read scene at given path
    std::ifstream f(scenePath);
    json data = json::parse(f);
}

void SceneManager::UnloadScene(std::string scenePath)
{
    
}
