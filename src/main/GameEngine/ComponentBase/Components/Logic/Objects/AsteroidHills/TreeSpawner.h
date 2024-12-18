#pragma once
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/ComponentBase/Components/Rendering/TextRenderer.h"

namespace component
{
    SERIALIZE_CLASS
    class TreeSpawner : public Component
    {
        MARK_SERIALIZABLE(TreeSpawner)

        TreeSpawner(transform::Transform* transform) : Component(transform)
        {
        }

        void Start() override;

        void MouseBtnPress(int mouseX, int mouseY, int button, int mods) override;

        void GenerateTree();
        
    public:
        SERIALIZE_FIELD int maxTrees = 5;

        float currSelectionValue = 1.1f;
        std::unordered_map<int, transform::Transform*> spawnedTrees;

        int collectedTrees = 0;
        TextRenderer* treesTextRenderer = nullptr;
    };
}
