#include "TransformVisualizer.h"

using namespace component;

void TransformVisualizer::Start()
{
    player = transform->GetTransformByTag("Player")->GetChild(0);
}

void TransformVisualizer::Update(float deltaTime)
{
    switch (direction)
    {
    case TransformDirectionForward:
        transform->SetLocalPosition(player->GetWorldPosition() + player->forward * distance);
        break;
        
    case TransformDirectionRight:
        transform->SetLocalPosition(player->GetWorldPosition() + player->right * distance);
        break;
        
    case TransformDirectionUp:
        transform->SetLocalPosition(player->GetWorldPosition() + player->up * distance);
        break;
    }
}
