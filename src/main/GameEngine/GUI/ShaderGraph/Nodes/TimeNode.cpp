#include "TimeNode.h"

#include <ctime>

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

std::string shader_graph::TimeNode::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    uniforms["time"] = "uniform float time;";
    
    return "time";
}
