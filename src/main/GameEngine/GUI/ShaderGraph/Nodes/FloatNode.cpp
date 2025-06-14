#include "FloatNode.h"

#include <iostream>
#include <sstream>

shader_graph::FloatNode::FloatNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Float, "", PinInteraction::Fixed, &value);
    AddPinToNode(PinKind::Output, PinType::Float, "Value");
}

const std::string& shader_graph::FloatNode::GetTypeName()
{
    static const std::string name = "Float";
    return name;
}

const std::string& shader_graph::FloatNode::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::FloatNode::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    const Pin& floatPin = inputs[0];
    std::string floatCode = floatPin.GenerateShaderCode(uniforms);

    return floatCode;
}
