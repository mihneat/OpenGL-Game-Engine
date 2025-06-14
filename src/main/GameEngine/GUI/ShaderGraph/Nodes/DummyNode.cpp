#include "DummyNode.h"

shader_graph::DummyNode::DummyNode(int id) : Node(id)
{
    AddPinToNode(PinKind::Input, PinType::Vector3, "Hi");
    AddPinToNode(PinKind::Output, PinType::Vector3, "Bye");
}

const std::string& shader_graph::DummyNode::GetTypeName()
{
    static const std::string name = "Dummy";
    return name;
}

const std::string& shader_graph::DummyNode::GetName() const
{
    return GetTypeName();
}

std::string shader_graph::DummyNode::GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms)
{
    return "\"dummy\"";
}
