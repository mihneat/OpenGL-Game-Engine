#include "FractalTreeRoot.h"

#include "FractalTreeSegment.h"
#include "main/GameEngine/PrefabManager.h"
#include "utils/memory_utils.h"

void component::FractalTreeRoot::Init(float newSelectionValue)
{
    selectionValue = newSelectionValue;
}

void component::FractalTreeRoot::Start()
{
    // Create a fractal tree segment
    auto segment = transform::Transform::Instantiate(prefabManager::PrefabManager::CreateFractalTreeSegment(transform));
    segment->GetComponent<FractalTreeSegment>()->Init(4, selectionValue);
    
    // Create a fractal tree segment
    auto segment2 = transform::Transform::Instantiate(prefabManager::PrefabManager::CreateFractalTreeSegment(transform));
    segment2->Translate(glm::vec3(0, 20, 0));
    segment2->SetScale(glm::vec3(0.9f, 1.0f, 0.9f));
    segment2->GetComponent<FractalTreeSegment>()->Init(3, selectionValue);
}
