#include "CosineNode.h"

shader_graph::CosineNode::CosineNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Float, "Radians", PinInteraction::Linkable, &value);
    AddPinToNode(PinKind::Output, PinType::Float, "Value");
}

const std::string& shader_graph::CosineNode::GetTypeName()
{
    static const std::string name = "Cosine";
    return name;
}

const std::string& shader_graph::CosineNode::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::CosineNode::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    const Pin& radsPin = inputs[0];
    std::string radsCode = radsPin.GenerateShaderCode(uniforms);
    
    return std::string("cos(" + radsCode + ")");
}
