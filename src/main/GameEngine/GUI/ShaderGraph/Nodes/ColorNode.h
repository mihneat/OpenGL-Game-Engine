#pragma once
#include "main/GameEngine/GUI/ShaderGraph/Node.h"

namespace shader_graph
{
    class ColorNode : public Node
    {
    public:
        ColorNode(int id);

        static const std::string& GetTypeName();
        const std::string& GetName() const override;

        float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
    };
}
