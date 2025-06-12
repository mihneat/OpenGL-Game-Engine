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
