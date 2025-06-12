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
