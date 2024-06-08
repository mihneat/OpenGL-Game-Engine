#include "GameInstance.h"

using namespace managers;

GameInstance* GameInstance::gameInstance = nullptr;

transform::Transform* GameInstance::Get()
{
    if (gameInstance == nullptr) {
        gameInstance = new GameInstance();
    }
    return gameInstance->attachedTransform;
}

void GameInstance::AttachTransform(transform::Transform* transform)
{
    attachedTransform = transform;
}
