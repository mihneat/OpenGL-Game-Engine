#include "ShaderGraphManager.h"

#include <iostream>
#include <fstream>

#include "Nodes/VertexShaderNode.h"

namespace ed = ax::NodeEditor;

using namespace shader_graph;

void ShaderGraphManager::ReadShaderGraphData()
{
    // Load all the other nodes from a separate file
    nodeDataPath = "assets/shader_graph/ShaderGraphData.sgd";
    std::ifstream fin(nodeDataPath);
    char line[512] = { };
    while (fin.getline(line, 512))
    {
        Node* node = Node::DeserializeNode(line);
        uniqueNodeId = std::max(uniqueNodeId, static_cast<int>(node->ID.Get()));
        
        // Add the node to the vector (skip the vertex and fragment shaders)
        if (node->ID.Get() == 1 || node->ID.Get() == 2)
        {
            delete node;
            continue;
        }
        
        // Add the node to the nodes vector
        graphNodes.emplace_back(*node);
        delete node;
    }
    
    fin.close();
}

void ShaderGraphManager::WriteShaderGraphData()
{
    // Write the node data to a separate file
    nodeDataPath = "assets/shader_graph/ShaderGraphData.sgd";
    std::ofstream fout(nodeDataPath);

    for (Node& node : graphNodes)
        fout << Node::SerializeNode(&node).c_str() << "\n";
    
    fout.close();
}

void ShaderGraphManager::Initialize()
{
    isInitialized = true;
    
    ed::Config config;
    config.SettingsFile = "assets/shader_graph/ShaderGraphConfig.json";
    config.UserPointer = this;
    
    config.LoadNodeSettings = [](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
    {
        auto self = static_cast<ShaderGraphManager*>(userPointer);

        auto node = self->FindNode(nodeId);
        if (!node)
        {
            std::cout << "Couldn't load node\n";
            return 0;
        }
        
        std::cout << "Loaded node\n";

        if (data != nullptr)
            memcpy(data, node->State.data(), node->State.size());
        return node->State.size();
    };

    config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
    {
        auto self = static_cast<ShaderGraphManager*>(userPointer);

        auto node = self->FindNode(nodeId);
        if (!node)
        {
            std::cout << "Couldn't save node\n";
            return false;
        }
        
        std::cout << "Saved node\n";

        node->State.assign(data, size);

        // self->TouchNode(nodeId);

        return true;
    };
    
    nodeEditorContext = ed::CreateEditor(&config);

    uniqueNodeId = 0;
    uniquePinId = 0;
    uniqueLinkId = 0;

    graphNodes.clear();
    
    // Build the initial nodes
    Node* vertexShaderNode = BuildNode(VertexShaderNode::GetTypeName());
    graphNodes.emplace_back(*vertexShaderNode);
    delete vertexShaderNode;
    
    Node* fragmentShaderNode = BuildNode(FragmentShaderNode::GetTypeName());
    graphNodes.emplace_back(*fragmentShaderNode);
    delete fragmentShaderNode;

    // Load the node data
    ReadShaderGraphData();

    // Rewrite the data (in case of first-time-open)
    WriteShaderGraphData();
}

void ShaderGraphManager::Deinitialize()
{
    isInitialized = false;
    
    if (nodeEditorContext != nullptr)
        ed::DestroyEditor(nodeEditorContext);

    nodeEditorContext = nullptr;
}

Node* ShaderGraphManager::FindNode(ed::NodeId id)
{
    if (!id)
        return nullptr;

    for (auto& node : graphNodes)
        if (node.ID == id)
            return &node;

    return nullptr;
}

Pin* ShaderGraphManager::FindPin(ed::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& node : graphNodes)
    {
        for (auto& pin : node.Inputs)
            if (pin.ID == id)
                return &pin;

        for (auto& pin : node.Outputs)
            if (pin.ID == id)
                return &pin;
    }

    return nullptr;
}

Node* ShaderGraphManager::BuildNode(const std::string& name)
{
    uniqueNodeId++;
    return Node::NodeFactory(name, uniqueNodeId);
}

Node* ShaderGraphManager::BuildDummyNode()
{
    Node* node = BuildEmptyNode("Dummy", ImColor(0, 0, 0));
    AddPinToNode(node, PinKind::Input, PinType::Vector3, "Hi");
    AddPinToNode(node, PinKind::Output, PinType::Vector3, "Bye");
    
    return node;
}

Node* ShaderGraphManager::BuildFragmentNode()
{
    Node* node = BuildEmptyNode("Fragment", ImColor(0, 255, 255));
    AddPinToNode(node, PinKind::Input, PinType::Vector4, "Albedo");
    AddPinToNode(node, PinKind::Input, PinType::Float, "Diffuse");
    AddPinToNode(node, PinKind::Input, PinType::Float, "Specular");
    AddPinToNode(node, PinKind::Input, PinType::Float, "Emission");

    return node;
}

void ShaderGraphManager::DrawNodePins(const Node& node)
{
    int i = 0;
    while (i < node.Inputs.size() && i < node.Outputs.size())
    {
        ed::BeginPin(node.Inputs[i].ID, ed::PinKind::Input);
        {
            ImGui::Text("O %s", node.Inputs[i].Name.c_str());
        } ed::EndPin();

        ImGui::SameLine();
        
        ed::BeginPin(node.Outputs[i].ID, ed::PinKind::Output);
        {
            ImGui::Text("%s O", node.Outputs[i].Name.c_str());
        } ed::EndPin();

        ++i;
    }
    
    while (i < node.Inputs.size())
    {
        ed::BeginPin(node.Inputs[i].ID, ed::PinKind::Input);
        {
            ImGui::Text("O %s", node.Inputs[i].Name.c_str());
        } ed::EndPin();

        ++i;
    }
    
    while (i < node.Outputs.size())
    {
        ed::BeginPin(node.Outputs[i].ID, ed::PinKind::Output);
        {
            ImGui::Text("%s O", node.Outputs[i].Name.c_str());
        } ed::EndPin();

        ++i;
    }
}

void ShaderGraphManager::DrawNode(const Node& node)
{
    ed::BeginNode(node.ID); {
        ImGui::Text("%s", node.Name.c_str());
        DrawNodePins(node);
    } ed::EndNode();
}

void ShaderGraphManager::DrawNodes()
{
    for (const Node& node : graphNodes)
        DrawNode(node);
}

void ShaderGraphManager::QueryLinks()
{
    if (createNewNode)
        return;

    auto currMousePos = ImGui::GetMousePos();
    
    if (ed::BeginCreate(ImVec4(0, 1, 1, 1), 3))
    {
        // TODO: This is used to link pins :)

        // ed::PinId startPinId = 0, endPinId = 0;
        // if (ed::QueryNewLink(&startPinId, &endPinId))
        // {
        //     auto startPin = FindPin(startPinId);
        //     auto endPin   = FindPin(endPinId);
        //
        //     newLinkPin = startPin ? startPin : endPin;
        //
        //     if (startPin->Kind == PinKind::Input)
        //     {
        //         std::swap(startPin, endPin);
        //         std::swap(startPinId, endPinId);
        //     }
        //
        //     if (startPin && endPin)
        //     {
        //         if (endPin == startPin)
        //         {
        //             ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
        //         }
        //         else if (endPin->Kind == startPin->Kind)
        //         {
        //             showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
        //             ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
        //         }
        //         //else if (endPin->Node == startPin->Node)
        //         //{
        //         //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
        //         //    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
        //         //}
        //         else if (endPin->Type != startPin->Type)
        //         {
        //             showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
        //             ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
        //         }
        //         else
        //         {
        //             showLabel("+ Create Link", ImColor(32, 45, 32, 180));
        //             if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
        //             {
        //                 m_Links.emplace_back(Link(GetNextId(), startPinId, endPinId));
        //                 m_Links.back().Color = GetIconColor(startPin->Type);
        //             }
        //         }
        //     }
        // }
        
        ed::PinId pinId = 0;
        if (ed::QueryNewNode(&pinId))
        {
            auto newLinkPin = FindPin(pinId);
            // if (newLinkPin)
                // showLabel("+ Create Node", ImColor(32, 45, 32, 180));
            

            if (ed::AcceptNewItem())
            {
                createNewNode = true;
                // newNodeLinkPin = FindPin(pinId);
                newLinkPin = nullptr;
                
                ed::Suspend();
                ImGui::OpenPopup("Create New Node");
                openPopupPosition = currMousePos;
                ed::Resume();
            }
        }
    } ed::EndCreate();
}

void ShaderGraphManager::DrawPopups()
{
    // ed::Suspend();
    // if (ed::ShowNodeContextMenu(nullptr))
    //     ImGui::OpenPopup("Node Context Menu");
    // else if (ed::ShowPinContextMenu(nullptr))
    //     ImGui::OpenPopup("Pin Context Menu");
    // else if (ed::ShowLinkContextMenu(nullptr))
    //     ImGui::OpenPopup("Link Context Menu");
    // else if (ed::ShowBackgroundContextMenu())
    // {
    //     ImGui::OpenPopup("Create New Node");
    //     // newNodeLinkPin = nullptr;
    // }
    // ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    // if (ImGui::BeginPopup("Node Context Menu"))
    // {
    //     auto node = FindNode(contextNodeId);
    //
    //     ImGui::TextUnformatted("Node Context Menu");
    //     ImGui::Separator();
    //     if (node)
    //     {
    //         ImGui::Text("ID: %p", node->ID.AsPointer());
    //         ImGui::Text("Type: %s", node->Type == NodeType::Blueprint ? "Blueprint" : (node->Type == NodeType::Tree ? "Tree" : "Comment"));
    //         ImGui::Text("Inputs: %d", (int)node->Inputs.size());
    //         ImGui::Text("Outputs: %d", (int)node->Outputs.size());
    //     }
    //     else
    //         ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
    //     ImGui::Separator();
    //     if (ImGui::MenuItem("Delete"))
    //         ed::DeleteNode(contextNodeId);
    //     ImGui::EndPopup();
    // }
    //
    // if (ImGui::BeginPopup("Pin Context Menu"))
    // {
    //     auto pin = FindPin(contextPinId);
    //
    //     ImGui::TextUnformatted("Pin Context Menu");
    //     ImGui::Separator();
    //     if (pin)
    //     {
    //         ImGui::Text("ID: %p", pin->ID.AsPointer());
    //         if (pin->Node)
    //             ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
    //         else
    //             ImGui::Text("Node: %s", "<none>");
    //     }
    //     else
    //         ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());
    //
    //     ImGui::EndPopup();
    // }
    //
    // if (ImGui::BeginPopup("Link Context Menu"))
    // {
    //     auto link = FindLink(contextLinkId);
    //
    //     ImGui::TextUnformatted("Link Context Menu");
    //     ImGui::Separator();
    //     if (link)
    //     {
    //         ImGui::Text("ID: %p", link->ID.AsPointer());
    //         ImGui::Text("From: %p", link->StartPinID.AsPointer());
    //         ImGui::Text("To: %p", link->EndPinID.AsPointer());
    //     }
    //     else
    //         ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
    //     ImGui::Separator();
    //     if (ImGui::MenuItem("Delete"))
    //         ed::DeleteLink(contextLinkId);
    //     ImGui::EndPopup();
    // }

    if (ImGui::BeginPopup("Create New Node"))
    {
        //ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

        //auto drawList = ImGui::GetWindowDrawList();
        //drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

        Node* node = nullptr;
        if (ImGui::MenuItem("Float"))
            node = BuildNode(VertexShaderNode::GetTypeName());
        if (ImGui::MenuItem("Vector2"))
            node = BuildNode("Dummy");
        if (ImGui::MenuItem("Vector3"))
            node = BuildNode("Dummy");
        if (ImGui::MenuItem("Vector4"))
            node = BuildNode("Dummy");
        if (ImGui::MenuItem("Color"))
            node = BuildNode("Dummy");
        ImGui::Separator();
        if (ImGui::MenuItem("Fresnel"))
            node = BuildNode("Dummy");

        if (node)
        {
            // BuildNodes();

            createNewNode = false;
            
            ed::SetNodePosition(node->ID, openPopupPosition);

            // Add the node to the nodes vector
            graphNodes.emplace_back(*node);
            delete node;

            WriteShaderGraphData();

            // if (auto startPin = newNodeLinkPin)
            // {
            //     auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;
            //
            //     for (auto& pin : pins)
            //     {
            //         if (CanCreateLink(startPin, &pin))
            //         {
            //             auto endPin = &pin;
            //             if (startPin->Kind == PinKind::Input)
            //                 std::swap(startPin, endPin);
            //
            //             m_Links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
            //             m_Links.back().Color = GetIconColor(startPin->Type);
            //
            //             break;
            //         }
            //     }
            // }
        }

        ImGui::EndPopup();
    }
    else
        createNewNode = false;
    
    ImGui::PopStyleVar();
    ed::Resume();
}

void ShaderGraphManager::HandleLinks()
{
    QueryLinks();
    DrawPopups();
}


void ShaderGraphManager::Draw()
{
    auto& io = ImGui::GetIO();

    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

    ImGui::Separator();

    ed::SetCurrentEditor(nodeEditorContext);
    ed::Begin("Shader Graph Editor", ImVec2(0.0, 0.0f));
    {
        DrawNodes();
        HandleLinks();
    } ed::End();
    
    ed::SetCurrentEditor(nullptr);

    ImGui::ShowMetricsWindow();
}

void ShaderGraphManager::GenerateShaderFiles()
{
    std::cout << "WORK IN PROGRESS" << "\n";
}
