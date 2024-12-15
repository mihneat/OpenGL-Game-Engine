#include "TreeSpawner.h"

#include "FractalTreeRoot.h"
#include "main/GameEngine/PrefabManager.h"
#include "utils/memory_utils.h"

void component::TreeSpawner::Start()
{
    for (int i = 0; i < maxTrees; ++i)
        GenerateTree();
}

int ComputeSelectionIndex(float selectionValue)
{
    return static_cast<int>(round(selectionValue * 10));
}

void component::TreeSpawner::GenerateTree()
{
    // Choose a point in the range [-500, 500] on both axis
    float x = (rand() * rand() % 3000 * 1.0f / 3000.0f - 0.5f) * 1000.0f;
    float z = (rand() * rand() % 3000 * 1.0f / 3000.0f - 0.5f) * 1000.0f;
    
    // Spawn in the tree
    const auto newTreeRoot = transform::Transform::Instantiate(prefabManager::PrefabManager::CreateFractalTreeRoot(transform));
    newTreeRoot->Translate(glm::vec3(x, 0, z));
    newTreeRoot->GetComponent<FractalTreeRoot>()->Init(currSelectionValue);

    spawnedTrees[ComputeSelectionIndex(currSelectionValue)] = newTreeRoot;
    
    currSelectionValue += 0.1f;
}

void component::TreeSpawner::MouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (!IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
        return;
    
    glm::vec4 selectionData = m1::GameEngine::ExtractSelectionData(mouseX, mouseY);
    std::cout << "Selected entity with value: " << selectionData << "\n";

    int selectionIndex = ComputeSelectionIndex(selectionData.a);
    if (spawnedTrees.find(selectionIndex) == spawnedTrees.end())
        return;

    std::cout << "Found a tree!\n";

    transform::Transform::Destroy(spawnedTrees[selectionIndex]);
    spawnedTrees.erase(spawnedTrees.find(selectionIndex));

    // Create a new tree
    GenerateTree();
}
