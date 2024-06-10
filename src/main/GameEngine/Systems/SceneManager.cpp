#include "SceneManager.h"

#include <fstream>
#include <iostream>

#include "main/GameEngine/Serialization/ObjectSerializer.h"

using namespace std;
using namespace transform;
using namespace component;
using json = nlohmann::json;

std::unordered_map<std::string, Transform *> SceneManager::loadedScenes;

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
    Transform* root = ObjectSerializer::DeserializeRootObject(json::parse(f));
    std::cout << "HEY\n";
}

void SceneManager::UnloadScene(std::string scenePath)
{
    
}
