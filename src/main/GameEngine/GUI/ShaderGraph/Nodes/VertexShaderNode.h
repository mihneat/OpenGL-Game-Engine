#pragma once
#include "main/GameEngine/GUI/ShaderGraph/Node.h"

namespace shader_graph
{
    class VertexShaderNode : public Node
    {
    public:
        VertexShaderNode(int id);

        static const std::string& GetTypeName();
        const std::string& GetName() const override;

        std::string GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms) override;
    };
}
