#pragma once
#include "main/GameEngine/GUI/ShaderGraph/Node.h"

namespace shader_graph
{
    class RemapNode : public Node
    {
    public:
        RemapNode(int id);

        static const std::string& GetTypeName();
        const std::string& GetName() const override;

        float value = 0.0f;
    };
}
