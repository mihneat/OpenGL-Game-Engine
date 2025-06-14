#include "SineNode.h"

shader_graph::SineNode::SineNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Float, "Radians", PinInteraction::Linkable, &value);
    AddPinToNode(PinKind::Output, PinType::Float, "Value");
}

const std::string& shader_graph::SineNode::GetTypeName()
{
    static const std::string name = "Sine";
    return name;
}

const std::string& shader_graph::SineNode::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::SineNode::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    const Pin& radsPin = inputs[0];
    std::string radsCode = radsPin.GenerateShaderCode(uniforms);
    
    return std::string("sin(" + radsCode + ")");
}
