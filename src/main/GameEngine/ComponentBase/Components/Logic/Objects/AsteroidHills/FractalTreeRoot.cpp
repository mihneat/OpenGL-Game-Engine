#include "FractalTreeRoot.h"

#include <iostream>

#include "FractalTreeSegment.h"
#include "main/GameEngine/PrefabManager.h"

void component::FractalTreeRoot::Init()
{

}

void component::FractalTreeRoot::Start()
{
    // Create a fractal tree segment
    auto segment = transform::Transform::Instantiate(prefabManager::PrefabManager::CreateFractalTreeSegment(transform));
    segment->GetComponent<FractalTreeSegment>()->Init(4);
}
