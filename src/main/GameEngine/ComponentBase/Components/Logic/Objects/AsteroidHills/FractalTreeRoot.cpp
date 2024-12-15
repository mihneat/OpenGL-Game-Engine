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
}
