#pragma once
#include "main/GameEngine/GUI/ShaderGraph/Node.h"

namespace shader_graph
{
    class FragmentShaderNode : public Node
    {
    public:
        FragmentShaderNode(int id);

        static const std::string& GetTypeName();
        const std::string& GetName() const override;

        std::string GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms) override;

        float diffuse = 1.0f;
        float specular = 1.0f;
        float emission = 1.0f;
    };
}
