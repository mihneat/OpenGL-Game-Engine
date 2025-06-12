#pragma once
#include "main/GameEngine/GUI/ShaderGraph/Node.h"

namespace shader_graph
{
    class DummyNode : public Node
    {
    public:
        DummyNode(int id);

        static const std::string& GetTypeName();
        const std::string& GetName() const override;
    };
}
