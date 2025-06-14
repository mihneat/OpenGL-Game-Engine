#include "VertexShaderNode.h"

#include <fstream>
#include <regex>

#include "core/managers/resource_path.h"
#include "main/GameEngine/Systems/FileSystem.h"
#include "utils/text_utils.h"

shader_graph::VertexShaderNode::VertexShaderNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Vector3, "Position");
}

const std::string& shader_graph::VertexShaderNode::GetTypeName()
{
    static const std::string name = "Vertex Shader";
    return name;
}

const std::string& shader_graph::VertexShaderNode::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::VertexShaderNode::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    // TODO: Grab the other required values

    // Form the final shader and return it
    std::string vertexShaderTemplatePath = PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::SHADER_GRAPH,
        "template\\TemplateGameEngine.VS.glsl");
    
    std::string vertexShaderCode;
    std::string line;
    
    std::ifstream fin(vertexShaderTemplatePath);
    while (std::getline(fin, line))
    {
        // Check for special lines
        if (line.find("{uniforms}") != std::string::npos)
        {
            for (const std::pair<const std::string, std::string>& uniform : uniforms)
                vertexShaderCode.append(uniform.second).append("\n");
            
            continue;
        }

        // Otherwise just copy the line
        vertexShaderCode.append(line + "\n");
    }

    fin.close();
    
    return vertexShaderCode;
}
