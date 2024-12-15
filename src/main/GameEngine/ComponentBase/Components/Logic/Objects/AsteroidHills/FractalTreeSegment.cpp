#include "FractalTreeSegment.h"

#include <iostream>

#include "main/GameEngine/PrefabManager.h"

void component::FractalTreeSegment::Init(int maxLevels)
{
    remainingLevels = maxLevels;
}

void component::FractalTreeSegment::Start()
{
    if (remainingLevels == 0)
        return;
    
    // Initialize more branches
    for (int i = 0; i < 3; ++i)
    {
        const auto newBranch = transform::Transform::Instantiate(prefabManager::PrefabManager::CreateFractalTreeSegment(transform));
        newBranch->GetComponent<FractalTreeSegment>()->Init(remainingLevels - 1);
        newBranch->Translate(glm::vec3(0, 18, 0));
        newBranch->Rotate(glm::vec3(0, i * 1.0f * glm::radians(120.0f), glm::radians(30.0f)));
        newBranch->SetScale(transform->GetLocalScale() * glm::vec3(0.85f, 0.95f, 0.85f));
    }

}
