#pragma once

#include <string>
#include <unordered_map>

namespace m1
{
    class GameEngine;
}

namespace transform
{
    class Transform;
}

namespace component
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
        static std::unordered_map<std::string, transform::Transform*> loadedScenes;
        static m1::GameEngine* engineRef;

        friend class m1::GameEngine;
    };
}

