#include "FragmentShaderNode.h"

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
