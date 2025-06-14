#include "Vector4Node.h"

shader_graph::Vector4Node::Vector4Node(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Float, "x", PinInteraction::Linkable, &x);
    AddPinToNode(PinKind::Input, PinType::Float, "y", PinInteraction::Linkable, &y);
    AddPinToNode(PinKind::Input, PinType::Float, "z", PinInteraction::Linkable, &z);
    AddPinToNode(PinKind::Input, PinType::Float, "w", PinInteraction::Linkable, &w);
    AddPinToNode(PinKind::Output, PinType::Vector4, "Vector4");
}

const std::string& shader_graph::Vector4Node::GetTypeName()
{
    static const std::string name = "Vector4";
    return name;
}

const std::string& shader_graph::Vector4Node::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::Vector4Node::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    const Pin& xPin = inputs[0];
    std::string xCode = xPin.GenerateShaderCode(uniforms);
    
    const Pin& yPin = inputs[1];
    std::string yCode = yPin.GenerateShaderCode(uniforms);
    
    const Pin& zPin = inputs[2];
    std::string zCode = zPin.GenerateShaderCode(uniforms);
    
    const Pin& wPin = inputs[3];
    std::string wCode = wPin.GenerateShaderCode(uniforms);
    
    return std::string("vec4(" + xCode + ", " + yCode + ", " + zCode + ", " + wCode + ")");
}
