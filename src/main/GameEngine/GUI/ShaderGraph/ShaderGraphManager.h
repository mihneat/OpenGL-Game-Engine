#pragma once

#include <string>
#include <vector>

#include "imgui_node_editor.h"
#include "Node.h"

namespace shader_graph
{
    class ShaderGraphManager
    {
    public:
        void ShowMenuBar();
        void RecountNodeUniqueIds();
        void RecountLinkUniqueIds();
        void ReadShaderGraphData();
        void WriteShaderGraphData();
        
        void Initialize();
        void Deinitialize();
    
        Node* FindNode(ax::NodeEditor::NodeId id);
        Pin* FindPin(ax::NodeEditor::PinId id);
        Node* BuildNode(const std::string& name);
        void DrawFloatSlider(const Pin& pin);
        void DrawInputPin(const Pin& pin);
        void DrawOutputPin(const Pin& pin);

        void DrawNodePins(const Node* node);
        void DrawNode(const Node* node);
        void DrawNodes();
        void RemoveLink(int linkId);
        void LinkPins(Pin& startPin, Pin& endPin);
        void DrawLinks();
        void QueryLinks();
        void DrawPopups();

        void HandleLinks();
        void Draw();

    
        void GenerateShaderFiles();

        bool isInitialized = false;

    private:
        ax::NodeEditor::EditorContext* nodeEditorContext = nullptr;

        int uniqueNodeId = 0;
        int uniquePinId = 0;
        int uniqueLinkId = 0;

        bool createNewNode = false;

        ImVec2 openPopupPosition = ImVec2(0, 0);

        std::vector<Node*> graphNodes;
        std::vector<Link*> graphLinks;

        std::string nodeDataPath;
        std::string nodeData;
    };
}
