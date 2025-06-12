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
