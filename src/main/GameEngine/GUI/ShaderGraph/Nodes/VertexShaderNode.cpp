#include "VertexShaderNode.h"

shader_graph::VertexShaderNode::VertexShaderNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Vector3, "Position");
}

std::string shader_graph::VertexShaderNode::GetTypeName()
{
    return "Vertex Shader";
}

std::string shader_graph::VertexShaderNode::GetName()
{
    return GetTypeName();
}
