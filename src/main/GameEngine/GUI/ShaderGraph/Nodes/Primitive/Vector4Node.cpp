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
