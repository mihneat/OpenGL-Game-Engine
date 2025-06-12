#pragma once
#include "main/GameEngine/GUI/ShaderGraph/Node.h"

namespace shader_graph
{
    class Vector2Node : public Node
    {
    public:
        Vector2Node(int id);

        static const std::string& GetTypeName();
        const std::string& GetName() const override;

        float x = 0.0f, y = 0.0f;
    };
}
