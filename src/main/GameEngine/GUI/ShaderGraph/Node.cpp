#include "Node.h"

#include <sstream>

#include "Nodes/VertexShaderNode.h"

using namespace shader_graph;

Node* Node::NodeFactory(const std::string& name, int id)
{
    if (name == VertexShaderNode::GetTypeName())
        return new VertexShaderNode(id);

    // if (name == "FragmentShader")
    //     return new FragmentShaderNode(id);

    return nullptr;
}

std::string Node::SerializeNode(Node* node)
{
    std::string buffer(std::to_string(node->ID.Get()));
    buffer.append("|").append(node->GetName());
    
    return buffer;
}

Node* Node::DeserializeNode(std::string line)
{
    if (line.empty())
        return nullptr;
    
    std::istringstream lineStream(line);
    std::string token;

    // Get the ID
    std::getline(lineStream, token, '|');
    int id = std::stoi(token);

    // Get the type
    std::getline(lineStream, token, '|');
    return NodeFactory(token, id);
}

void Node::AddPinToNode(PinKind pinKind, PinType pinType, const char* name)
{
    switch (pinKind)
    {
    case PinKind::Input:
        Inputs.emplace_back(++startPinId, name, pinType);
        break;

    case PinKind::Output:
        Outputs.emplace_back(++startPinId, name, pinType);
        break;
    }
}
