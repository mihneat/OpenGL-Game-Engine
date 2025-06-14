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

        std::string GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms) override;
    };
}
