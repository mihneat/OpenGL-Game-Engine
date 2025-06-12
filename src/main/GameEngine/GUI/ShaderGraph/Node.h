#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "imgui_node_editor.h"

#define PIN_ID_OFFSET 100

namespace shader_graph
{
    enum class PinType
    {
        Bool,
        Int,
        Float,
        Vector2,
        Vector3,
        Vector4,
        Color,
        Texture2D
    };

    enum class PinKind
    {
        Input,
        Output
    };

    enum class PinInteraction
    {
        Linkable,
        Fixed
    };

    struct Pin
    {
        ax::NodeEditor::PinId id;
        class Node* node;
        std::string name;
        PinType type;
        PinKind kind;
        PinInteraction interaction;

        struct Link* link = nullptr;

        void* value;

        Pin(int id, const char* name, PinType type):
            id(id), node(nullptr), name(name), type(type), kind(PinKind::Input),
            interaction(PinInteraction::Linkable), value(nullptr)
        {
        }

        Pin(int id, const char* name, PinType type, PinInteraction interaction, void* val):
            id(id), node(nullptr), name(name), type(type), kind(PinKind::Input),
            interaction(interaction), value(val)
        {
        }
        
        std::string Serialize() const;
        std::string Deserialize(std::string line);
    };

    struct Link
    {
        ax::NodeEditor::LinkId id;

        ax::NodeEditor::PinId startPinID;
        ax::NodeEditor::PinId endPinID;

        ImColor color;

        Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId):
            id(id), startPinID(startPinId), endPinID(endPinId), color(255, 255, 255)
        {
        }
    };

    enum class NodeType
    {
        Primitive,
        Vertex,
        Fragment
    };

    class Node
    {
    public:
        ax::NodeEditor::NodeId id;
        std::string name;
        std::vector<Pin> inputs;
        std::vector<Pin> outputs;
        ImColor color;
        NodeType type;
        ImVec2 size;

        std::string state;
        std::string savedState;

        Node(int id): id(id), color(ImVec4(0, 0, 0, 1)), type(NodeType::Primitive), size(0, 0)
        {
            startPinId = id * PIN_ID_OFFSET;
        }

        virtual ~Node() = default;

        virtual const std::string& GetName() const = 0;

        static Node* NodeFactory(const std::string& name, int id);
        static std::string SerializeNode(Node* node);
        static Node* DeserializeNode(std::string line);
        
        std::string SerializePins();
        void DeserializePins(std::string line);

        // TODO: Make this a pure virtual method after implementing a few nodes
        virtual std::string GenerateShaderCode(std::unordered_map<std::string, std::string>& uniforms);

    protected:
        void AddPinToNode(PinKind pinKind, PinType pinType, const char* name,
            PinInteraction pinInteraction = PinInteraction::Linkable, void* value = nullptr);
        int startPinId;
    };
}
