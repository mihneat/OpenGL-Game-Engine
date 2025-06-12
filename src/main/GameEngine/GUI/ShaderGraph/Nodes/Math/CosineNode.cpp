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
