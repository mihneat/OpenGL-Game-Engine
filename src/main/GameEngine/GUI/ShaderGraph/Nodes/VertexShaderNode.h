#pragma once
#include "main/GameEngine/GUI/ShaderGraph/Node.h"

namespace shader_graph
{
    class VertexShaderNode : public Node
    {
    public:
        VertexShaderNode(int id);

        static std::string GetTypeName();
        std::string GetName() override;
    };
}
