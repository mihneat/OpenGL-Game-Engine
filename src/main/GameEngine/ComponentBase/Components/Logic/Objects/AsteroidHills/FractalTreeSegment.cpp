#include "FractalTreeSegment.h"

#include <iostream>

#include "main/GameEngine/PrefabManager.h"

void component::FractalTreeSegment::Init(int newRemainingLevels, float newSelectionValue)
{
    remainingLevels = newRemainingLevels;
    selectionValue = newSelectionValue;

    const auto matOverrides = new rendering::MaterialOverrides();
    matOverrides->ints["distance_from_leaf"] = newRemainingLevels;
    matOverrides->floats["selection_value"] = newSelectionValue;
    
    meshRenderer = transform->GetChild(0)->GetComponent<MeshRenderer>();
    meshRenderer->SetMaterialOverrides(matOverrides);

    if (newRemainingLevels == 0)
    {
        meshRenderer->SetMesh(MeshRenderer::Cube);
        transform->GetChild(0)->SetScale(glm::vec3(20.0f));
    }
}

void component::FractalTreeSegment::Start()
{
    if (remainingLevels == 0)
        return;
    
    // Initialize more branches
    for (int i = 0; i < 3; ++i)
    {
        const float shouldSkipChance = rand() % 1001 / 10.0f;
        if (shouldSkipChance < 1.05f)
            continue;
        
        const float maxLowerHeight = rand() % 100 / 100.0f * 6.0f;
        const float maxExtraRotation = rand() % 100 / 100.0f * 40.0f - 20.0f;
        const float maxExtraRotation2 = rand() % 100 / 100.0f * 30.0f - 15.0f;
        
        const auto newBranch = transform::Transform::Instantiate(prefabManager::PrefabManager::CreateFractalTreeSegment(transform));
        newBranch->GetComponent<FractalTreeSegment>()->Init(remainingLevels - 1, selectionValue);
        newBranch->Translate(glm::vec3(0.0f, 19.0f - maxLowerHeight, 0.0f));
        newBranch->Rotate(glm::vec3(0, i * 1.0f * glm::radians(120.0f + maxExtraRotation), glm::radians(30.0f + maxExtraRotation2)));
        newBranch->SetScale(transform->GetLocalScale() * glm::vec3(0.85f, 0.95f, 0.85f));
    }

}
