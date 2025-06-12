#include "Node.h"

#include <sstream>

#include "Nodes/ColorNode.h"
#include "Nodes/DummyNode.h"
#include "Nodes/FloatNode.h"
#include "Nodes/FragmentShaderNode.h"
#include "Nodes/TimeNode.h"
#include "Nodes/VertexShaderNode.h"
#include "Nodes/Math/CosineNode.h"
#include "Nodes/Math/RemapNode.h"
#include "Nodes/Math/SineNode.h"
#include "Nodes/Primitive/Vector2Node.h"
#include "Nodes/Primitive/Vector3Node.h"
#include "Nodes/Primitive/Vector4Node.h"

#define BUF_SIZE 64

using namespace shader_graph;

std::string Pin::Serialize() const
{
    if (value == nullptr)
        return "";
    
    std::string attributes;
    switch (type)
    {
    case PinType::Float:
        attributes.append(std::to_string(*static_cast<float*>(value)));

        attributes.append("|");
        return attributes;

    default:
        return "";
    }
}

std::string Pin::Deserialize(std::string line)
{
    if (value == nullptr)
        return line;
    
    if (line.empty())
        return line;
    
    std::istringstream lineStream(line);
    std::string token;
    switch (type)
    {
    case PinType::Float:
        std::getline(lineStream, token, '|');
        *static_cast<float*>(value) = std::stof(token);

        std::getline(lineStream, token);
        return token;

    default:
        return line;
    }
}

Node* Node::NodeFactory(const std::string& name, int id)
{
    // Base nodes
    if (name == VertexShaderNode::GetTypeName())
        return new VertexShaderNode(id);

    if (name == FragmentShaderNode::GetTypeName())
        return new FragmentShaderNode(id);

    // Primitive nodes
    if (name == FloatNode::GetTypeName())
        return new FloatNode(id);

    if (name == Vector2Node::GetTypeName())
        return new Vector2Node(id);

    if (name == Vector3Node::GetTypeName())
        return new Vector3Node(id);

    if (name == Vector4Node::GetTypeName())
        return new Vector4Node(id);

    if (name == ColorNode::GetTypeName())
        return new ColorNode(id);

    // Engine nodes
    if (name == TimeNode::GetTypeName())
        return new TimeNode(id);

    // Math nodes
    if (name == SineNode::GetTypeName())
        return new SineNode(id);

    if (name == CosineNode::GetTypeName())
        return new CosineNode(id);

    if (name == RemapNode::GetTypeName())
        return new RemapNode(id);

    // Dummy nodes
    if (name == DummyNode::GetTypeName())
        return new DummyNode(id);

    return nullptr;
}

std::string Node::SerializeNode(Node* node)
{
    std::string buffer(std::to_string(node->id.Get()));
    buffer.append("|").append(node->GetName())
        .append("|").append(node->SerializePins());
    
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

    Node* newNode = NodeFactory(token, id);

    std::getline(lineStream, token);
    newNode->DeserializePins(token);

    return newNode;
}

std::string Node::SerializePins()
{
    std::string serializedPins;
    for (const Pin& pin : inputs)
        serializedPins.append(pin.Serialize());
    
    for (const Pin& pin : outputs)
        serializedPins.append(pin.Serialize());

    return serializedPins;
}

void Node::DeserializePins(std::string line)
{
    for (Pin& pin : inputs)
        line = pin.Deserialize(line);
    
    for (Pin& pin : outputs)
        line = pin.Deserialize(line);
}

void Node::AddPinToNode(PinKind pinKind, PinType pinType, const char* pinName, PinInteraction pinInteraction, void* value)
{
    switch (pinKind)
    {
    case PinKind::Input:
        inputs.emplace_back(++startPinId, pinName, pinType, pinInteraction, value);
        inputs.back().node = this;
        inputs.back().kind = pinKind;
        break;

    case PinKind::Output:
        outputs.emplace_back(++startPinId, pinName, pinType);
        outputs.back().node = this;
        outputs.back().kind = pinKind;
        break;
    }
}
