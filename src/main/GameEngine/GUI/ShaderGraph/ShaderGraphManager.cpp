#include "ShaderGraphManager.h"

#include <iostream>
#include <fstream>
#include <glm/common.hpp>

#include "imgui_internal.h"
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
        uniqueNodeId = std::max(uniqueNodeId, static_cast<int>(node->id.Get()));
        
        // Add the node to the vector (skip the vertex and fragment shaders)
        if (node->id.Get() == 1 || node->id.Get() == 2)
        {
            delete node;
            continue;
        }
        
        // Add the node to the nodes vector
        graphNodes.emplace_back(node);
    }
    
    fin.close();
}

void ShaderGraphManager::WriteShaderGraphData()
{
    // Write the node data to a separate file
    nodeDataPath = "assets/shader_graph/ShaderGraphData.sgd";
    std::ofstream fout(nodeDataPath);

    for (Node* node : graphNodes)
        fout << Node::SerializeNode(node).c_str() << "\n";
    
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
            memcpy(data, node->state.data(), node->state.size());
        return node->state.size();
    };

    config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
    {
        auto self = static_cast<ShaderGraphManager*>(userPointer);

        auto node = self->FindNode(nodeId);
        if (!node)
        {
            // std::cout << "Couldn't save node\n";
            return false;
        }
        
        // std::cout << "Saved node\n";

        node->state.assign(data, size);

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
    graphNodes.emplace_back(vertexShaderNode);
    
    Node* fragmentShaderNode = BuildNode(FragmentShaderNode::GetTypeName());
    graphNodes.emplace_back(fragmentShaderNode);

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
        if (node->id == id)
            return node;

    return nullptr;
}

Pin* ShaderGraphManager::FindPin(ed::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& node : graphNodes)
    {
        for (auto& pin : node->inputs)
            if (pin.id == id)
                return &pin;

        for (auto& pin : node->outputs)
            if (pin.id == id)
                return &pin;
    }

    return nullptr;
}

Node* ShaderGraphManager::BuildNode(const std::string& name)
{
    uniqueNodeId++;
    return Node::NodeFactory(name, uniqueNodeId);
}

void ShaderGraphManager::DrawFloatSlider(const Pin& pin)
{
    if (pin.value == nullptr)
        return;

    static unsigned int lastFocusId = ImGui::GetFocusID();

    static char buf[BUF_SIZE] = {};
    memset(buf, 0, BUF_SIZE);
    snprintf(buf, BUF_SIZE - 1, "##Pin %d", pin.id.Get());

    
    ImGui::PushItemWidth(80);
    
    if (ImGui::DragFloat(buf, static_cast<float*>(pin.value), 0.02f))
        lastFocusId = ImGui::GetFocusID();

    if (lastFocusId != ImGui::GetFocusID())
    {
        // TODO: This system is not perfect. It only saves when focus on the slider is LOST
        // This works for now but I'd like to change this at some point :)
        // The workaround is very easy for now, it only requires you to click anywhere else
        // on the canvas and the values are saved
        lastFocusId = ImGui::GetFocusID();
        WriteShaderGraphData();
    }

    ImGui::PopItemWidth();
}

void ShaderGraphManager::DrawInputPin(const Pin& pin)
{
    ed::BeginPin(pin.id, ed::PinKind::Input);
    {
        if (pin.isLinked)
        {
            ImGui::Text("O %s", pin.name.c_str());
        } else
        {
            switch (pin.type)
            {
            case PinType::Float:
                ImGui::Text("O ");

                ImGui::SameLine();
                DrawFloatSlider(pin);

                ImGui::SameLine();
                ImGui::Text("%s", pin.name.c_str());
            
                break;

            default:
                ImGui::Text("O %s", pin.name.c_str());
                break;
            }
        }
    } ed::EndPin();
}

void ShaderGraphManager::DrawOutputPin(const Pin& pin)
{
    ed::BeginPin(pin.id, ed::PinKind::Output);
    {
        ImGui::Text("%s O", pin.name.c_str());
    } ed::EndPin();
}

void ShaderGraphManager::DrawNodePins(const Node* node)
{
    int i = 0;
    while (i < static_cast<int>(node->inputs.size()) && i < static_cast<int>(node->outputs.size()))
    {
        DrawInputPin(node->inputs[i]);
        ImGui::SameLine();
        DrawOutputPin(node->outputs[i]);

        ++i;
    }
    
    while (i < static_cast<int>(node->inputs.size()))
    {
        DrawInputPin(node->inputs[i]);

        ++i;
    }
    
    
    while (i < static_cast<int>(node->outputs.size()))
    {
        DrawOutputPin(node->outputs[i]);

        ++i;
    }
}

void ShaderGraphManager::DrawNode(const Node* node)
{
    ed::BeginNode(node->id); {
        ImGui::Text("%s", node->GetName().c_str());
        DrawNodePins(node);
    } ed::EndNode();
}

void ShaderGraphManager::DrawNodes()
{
    for (Node* node : graphNodes)
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

    if (ed::BeginDelete())
    {
        ed::NodeId nodeId = 0;
        while (ed::QueryDeletedNode(&nodeId))
        {
            // Do not destroy the vertex and fragment shader nodes
            if (nodeId.Get() == 1 || nodeId.Get() == 2)
                continue;
            
            if (ed::AcceptDeletedItem())
            {
                auto id = std::find_if(graphNodes.begin(), graphNodes.end(), [nodeId](const Node* node) { return node->id == nodeId; });
                if (id == graphNodes.end())
                    continue;
                    
                graphNodes.erase(id);

                WriteShaderGraphData();

                // Reset unique ID to the largest remaining one
                auto maxIndex = std::max_element(graphNodes.begin(), graphNodes.end(),
                    [](const Node* node1, const Node* node2)
                    {
                        int id1 = static_cast<int>(node1->id.Get());
                        int id2 = static_cast<int>(node2->id.Get());

                        return id1 < id2;
                    });
                if (maxIndex == graphNodes.end())
                    uniqueNodeId = 1;
                else
                    uniqueNodeId = static_cast<int>((*maxIndex)->id.Get());
            }
        }

        // ed::LinkId linkId = 0;
        // while (ed::QueryDeletedLink(&linkId))
        // {
        //     if (ed::AcceptDeletedItem())
        //     {
        //         auto id = std::find_if(m_Links.begin(), m_Links.end(), [linkId](auto& link) { return link.ID == linkId; });
        //         if (id != m_Links.end())
        //             m_Links.erase(id);
        //     }
        // }
    } ed::EndDelete();
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
        
        // Primitives
        if (ImGui::MenuItem("Float"))
            node = BuildNode(FloatNode::GetTypeName());
        if (ImGui::MenuItem("Vector2"))
            node = BuildNode(Vector2Node::GetTypeName());
        if (ImGui::MenuItem("Vector3"))
            node = BuildNode(Vector3Node::GetTypeName());
        if (ImGui::MenuItem("Vector4"))
            node = BuildNode(Vector4Node::GetTypeName());
        if (ImGui::MenuItem("Color"))
            node = BuildNode(ColorNode::GetTypeName());
        
        // Math
        ImGui::Separator();
        if (ImGui::MenuItem("Sine"))
            node = BuildNode(SineNode::GetTypeName());
        if (ImGui::MenuItem("Cosine"))
            node = BuildNode(CosineNode::GetTypeName());
        if (ImGui::MenuItem("Remap"))
            node = BuildNode(RemapNode::GetTypeName());
        
        // Engine
        ImGui::Separator();
        if (ImGui::MenuItem("Time"))
            node = BuildNode(TimeNode::GetTypeName());
        
        // Special
        ImGui::Separator();
        if (ImGui::MenuItem("Fresnel"))
            node = BuildNode(DummyNode::GetTypeName());

        if (node)
        {
            // BuildNodes();

            createNewNode = false;
            
            ed::SetNodePosition(node->id, openPopupPosition);

            // Add the node to the nodes vector
            graphNodes.emplace_back(node);

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
