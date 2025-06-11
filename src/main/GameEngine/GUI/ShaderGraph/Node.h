#pragma once
#include <string>
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

    struct Pin
    {
        ax::NodeEditor::PinId   ID;
        class Node* Node;
        std::string Name;
        PinType     Type;
        PinKind     Kind;

        Pin(int id, const char* name, PinType type):
            ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
        {
        }
    };

    struct Link
    {
        ax::NodeEditor::LinkId ID;

        ax::NodeEditor::PinId StartPinID;
        ax::NodeEditor::PinId EndPinID;

        ImColor Color;

        Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId):
            ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
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
        ax::NodeEditor::NodeId ID;
        std::string Name;
        std::vector<Pin> Inputs;
        std::vector<Pin> Outputs;
        ImColor Color;
        NodeType Type;
        ImVec2 Size;

        std::string State;
        std::string SavedState;

        Node(int id): ID(id), Color(ImVec4(0, 0, 0, 1)), Type(NodeType::Primitive), Size(0, 0)
        {
            startPinId = id * PIN_ID_OFFSET;
        }

        virtual ~Node() = default;

        virtual std::string GetName() = 0;

        static Node* NodeFactory(const std::string& name, int id);
        static std::string SerializeNode(Node* node);
        static Node* DeserializeNode(std::string line);

    protected:
        void AddPinToNode(PinKind pinKind, PinType pinType, const char* name);

    private:
        int startPinId;
    };
}
