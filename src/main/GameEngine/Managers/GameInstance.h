#pragma once
#include "main/GameEngine/GameEngine.h"

class ObjectSerializer;

namespace managers
{
    class GameInstance
    {
    protected:
        GameInstance() { }

        static GameInstance* gameInstance;

    public:
        GameInstance(GameInstance& other) = delete;
        void operator=(const GameInstance&) = delete;

        static transform::Transform* Get();

    private:
        void AttachTransform(transform::Transform* transform);

        transform::Transform* attachedTransform = nullptr;
        
        friend class m1::GameEngine;
        friend class ObjectSerializer;
    };
}
