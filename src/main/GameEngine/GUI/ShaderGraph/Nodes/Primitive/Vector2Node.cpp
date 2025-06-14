#include "Vector2Node.h"

shader_graph::Vector2Node::Vector2Node(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Float, "x", PinInteraction::Linkable, &x);
    AddPinToNode(PinKind::Input, PinType::Float, "y", PinInteraction::Linkable, &y);
    AddPinToNode(PinKind::Output, PinType::Vector2, "Vector2");
}

const std::string& shader_graph::Vector2Node::GetTypeName()
{
    static const std::string name = "Vector2";
    return name;
}

const std::string& shader_graph::Vector2Node::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::Vector2Node::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    const Pin& xPin = inputs[0];
    std::string xCode = xPin.GenerateShaderCode(uniforms);
    
    const Pin& yPin = inputs[1];
    std::string yCode = yPin.GenerateShaderCode(uniforms);
    
    return std::string("vec2(" + xCode + ", " + yCode + ")");
}
