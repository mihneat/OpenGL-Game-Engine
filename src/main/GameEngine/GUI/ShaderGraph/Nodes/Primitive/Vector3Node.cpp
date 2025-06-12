#include "Vector3Node.h"

shader_graph::Vector3Node::Vector3Node(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Float, "x", PinInteraction::Linkable, &x);
    AddPinToNode(PinKind::Input, PinType::Float, "y", PinInteraction::Linkable, &y);
    AddPinToNode(PinKind::Input, PinType::Float, "z", PinInteraction::Linkable, &z);
    AddPinToNode(PinKind::Output, PinType::Vector3, "Vector3");
}

const std::string& shader_graph::Vector3Node::GetTypeName()
{
    static const std::string name = "Vector3";
    return name;
}

const std::string& shader_graph::Vector3Node::GetName() const
{
    return GetTypeName();
}
