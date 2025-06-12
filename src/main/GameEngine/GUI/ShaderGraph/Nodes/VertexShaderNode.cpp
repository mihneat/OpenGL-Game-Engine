#include "VertexShaderNode.h"

shader_graph::VertexShaderNode::VertexShaderNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Vector3, "Position");
}

const std::string& shader_graph::VertexShaderNode::GetTypeName()
{
    static const std::string name = "Vertex Shader";
    return name;
}

const std::string& shader_graph::VertexShaderNode::GetName() const
{
    return GetTypeName();
}
