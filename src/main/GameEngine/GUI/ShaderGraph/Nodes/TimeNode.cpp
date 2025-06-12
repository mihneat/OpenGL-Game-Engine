#include "TimeNode.h"

shader_graph::TimeNode::TimeNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Output, PinType::Float, "Current time (s)");
}

const std::string& shader_graph::TimeNode::GetTypeName()
{
    static const std::string name = "Time";
    return name;
}

const std::string& shader_graph::TimeNode::GetName() const
{
    return GetTypeName();
}
