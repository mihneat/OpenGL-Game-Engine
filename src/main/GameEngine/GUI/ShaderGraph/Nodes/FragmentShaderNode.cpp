#include "FragmentShaderNode.h"

#include <fstream>

#include "core/managers/resource_path.h"
#include "main/GameEngine/Systems/FileSystem.h"
#include "utils/text_utils.h"

shader_graph::FragmentShaderNode::FragmentShaderNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Color, "Albedo");
    AddPinToNode(PinKind::Input, PinType::Float, "Diffuse", PinInteraction::Linkable, &diffuse);
    AddPinToNode(PinKind::Input, PinType::Float, "Specular", PinInteraction::Linkable, &specular);
    AddPinToNode(PinKind::Input, PinType::Float, "Emission", PinInteraction::Linkable, &emission);
}

const std::string& shader_graph::FragmentShaderNode::GetTypeName()
{
    static const std::string name = "Fragment Shader";
    return name;
}

const std::string& shader_graph::FragmentShaderNode::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::FragmentShaderNode::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    // Grab the other required values
    const Pin& albedoPin = inputs[0];
    std::string albedoCode = "    shader_color = ";
    albedoCode.append(albedoPin.GenerateShaderCode(uniforms)).append(";\n");

    // Form the final shader and return it
    std::string fragmentShaderTemplatePath = PATH_JOIN(FileSystem::rootDirectory, RESOURCE_PATH::SHADER_GRAPH,
        "template\\TemplateGameEngine.FS.glsl");
    
    std::string fragmentShaderCode;
    std::string line;
    
    std::ifstream fin(fragmentShaderTemplatePath);
    while (std::getline(fin, line))
    {
        // Check for special lines
        if (line.find("{uniforms}") != std::string::npos)
        {
            for (const std::pair<const std::string, std::string>& uniform : uniforms)
                fragmentShaderCode.append(uniform.second).append("\n");
            
            continue;
        }
        
        if (line.find("{color}") != std::string::npos)
        {
            fragmentShaderCode.append(albedoCode);
            continue;
        }
        
        fragmentShaderCode.append(line + "\n");
    }

    fin.close();
    
    return fragmentShaderCode;
}
