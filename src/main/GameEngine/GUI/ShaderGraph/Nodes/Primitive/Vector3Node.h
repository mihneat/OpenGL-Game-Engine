#pragma once
#include "main/GameEngine/GUI/ShaderGraph/Node.h"

namespace shader_graph
{
    class Vector3Node : public Node
    {
    public:
        Vector3Node(int id);

        static const std::string& GetTypeName();
        const std::string& GetName() const override;

        std::string GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms) override;

        float x = 0.0f, y = 0.0f, z = 0.0f;
    };
}
