#pragma once
#include "main/GameEngine/GUI/ShaderGraph/Node.h"

namespace shader_graph
{
    class Vector4Node : public Node
    {
    public:
        Vector4Node(int id);

        static const std::string& GetTypeName();
        const std::string& GetName() const override;

        std::string GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms) override;

        float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
    };
}
