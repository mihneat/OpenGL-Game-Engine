#include "ColorNode.h"

shader_graph::ColorNode::ColorNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Float, "R", PinInteraction::Linkable, &r);
    AddPinToNode(PinKind::Input, PinType::Float, "G", PinInteraction::Linkable, &g);
    AddPinToNode(PinKind::Input, PinType::Float, "B", PinInteraction::Linkable, &b);
    AddPinToNode(PinKind::Input, PinType::Float, "A", PinInteraction::Linkable, &a);
    AddPinToNode(PinKind::Output, PinType::Color, "Color");
}

const std::string& shader_graph::ColorNode::GetTypeName()
{
    static const std::string name = "Color";
    return name;
}

const std::string& shader_graph::ColorNode::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::ColorNode::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    const Pin& rPin = inputs[0];
    std::string rCode = rPin.GenerateShaderCode(uniforms);
    
    const Pin& gPin = inputs[1];
    std::string gCode = gPin.GenerateShaderCode(uniforms);
    
    const Pin& bPin = inputs[2];
    std::string bCode = bPin.GenerateShaderCode(uniforms);
    
    const Pin& aPin = inputs[3];
    std::string aCode = aPin.GenerateShaderCode(uniforms);
    
    return std::string("vec4(" + rCode + ", " + gCode + ", " + bCode + ", " + aCode + ")");
}
