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
