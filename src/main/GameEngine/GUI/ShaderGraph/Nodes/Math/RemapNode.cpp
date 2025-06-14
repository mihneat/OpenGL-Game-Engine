#include "RemapNode.h"

shader_graph::RemapNode::RemapNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Float, "In", PinInteraction::Linkable, &value);
    AddPinToNode(PinKind::Input, PinType::Vector2, "In interval");
    AddPinToNode(PinKind::Input, PinType::Vector2, "Out interval");
    AddPinToNode(PinKind::Output, PinType::Float, "Out");
}

const std::string& shader_graph::RemapNode::GetTypeName()
{
    static const std::string name = "Remap";
    return name;
}

const std::string& shader_graph::RemapNode::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::RemapNode::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    const Pin& inPin = inputs[0];
    std::string inCode = inPin.GenerateShaderCode(uniforms);
    
    const Pin& inIntervalPin = inputs[1];
    std::string inIntervalCode = inIntervalPin.GenerateShaderCode(uniforms);
    
    const Pin& outIntervalPin = inputs[2];
    std::string outIntervalCode = outIntervalPin.GenerateShaderCode(uniforms);
    
    return std::string("remap(" + inCode + ", " + inIntervalCode + ", " + outIntervalCode + ")");
}
