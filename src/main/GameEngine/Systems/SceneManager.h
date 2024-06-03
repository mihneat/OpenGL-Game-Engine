#pragma once

#include <string>
#include <unordered_map>

namespace transform
{
    class Transform;
}

namespace managers
{
    enum SceneLoadMode
    {
        Single,
        Additive,
    };
    
    class SceneManager
    {
    public:
        SceneManager() = default;
        ~SceneManager() = default;

        static void LoadScene(std::string scenePath, SceneLoadMode mode = Single);
        static void UnloadScene(std::string scenePath);

    protected:
        static std::unordered_map<std::string, transform::Transform *> loadedScenes;
    };
}

