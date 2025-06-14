#include "ShaderGraphManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <glm/common.hpp>

#include "imgui_internal.h"
#include "core/managers/resource_path.h"
#include "main/GameEngine/GUI/GUIManager.h"
#include "main/GameEngine/Serialization/CppHeaderParser.h"
#include "main/GameEngine/Systems/FileSystem.h"
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
#include "utils/text_utils.h"

#define BUF_SIZE 64

namespace ed = ax::NodeEditor;

using namespace shader_graph;

void ShaderGraphManager::ShowMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(FLT_MAX, 350));
        if (ImGui::BeginMenu("File", true))
        {
            if (ImGui::MenuItem("Generate", "CTRL+G", nullptr))
                GenerateShaderFiles();
                
            ImGui::EndMenu();
        }
            
        ImGui::EndMenuBar();
    }
}

void ShaderGraphManager::RecountNodeUniqueIds()
{
    // Reset unique ID to the largest remaining one
    auto maxIndex = std::max_element(graphNodes.begin(), graphNodes.end(),
        [](const Node* node1, const Node* node2)
        {
            int id1 = static_cast<int>(node1->id.Get());
            int id2 = static_cast<int>(node2->id.Get());

            return id1 < id2;
        });
    if (maxIndex == graphNodes.end())
        uniqueNodeId = 0;
    else
        uniqueNodeId = static_cast<int>((*maxIndex)->id.Get());
}

void ShaderGraphManager::RecountLinkUniqueIds()
{
    auto maxIndex = std::max_element(graphLinks.begin(), graphLinks.end(),
        [](const Link* link1, const Link* link2)
        {
            int id1 = static_cast<int>(link1->id.Get());
            int id2 = static_cast<int>(link2->id.Get());

            return id1 < id2;
        });
    if (maxIndex == graphLinks.end())
        uniqueLinkId = 0;
    else
        uniqueLinkId = static_cast<int>((*maxIndex)->id.Get());
}

void ShaderGraphManager::ReadShaderGraphData()
{
    // Load all the other nodes from a separate file
    nodeDataPath = "assets/shader_graph/ShaderGraphData.sgd";
    std::ifstream fin(nodeDataPath);
    
    char line[2048] = { };
    while (fin.getline(line, 2048))
    {
        // Check if the line holds the serialization of a node or a link
        std::istringstream lineStream(line);
        std::string token;
        
        std::getline(lineStream, token, '|');
        char lineType = token[0];

        // Read the rest of the line
        std::getline(lineStream, token);

        if (lineType == 'n')
        {
            Node* node = Node::DeserializeNode(token);
            uniqueNodeId = std::max(uniqueNodeId, static_cast<int>(node->id.Get()));
        
            // Add the node to the vector (skip the vertex and fragment shaders)
            if (node->id.Get() == 1 || node->id.Get() == 2)
            {
                delete node;
                continue;
            }
        
            // Add the node to the nodes vector
            graphNodes.emplace_back(node);
        } else if (lineType == 'l')
        {
            lineStream = std::istringstream(token);
            
            std::getline(lineStream, token, '|');
            int startPinId = std::stoi(token);
            
            std::getline(lineStream, token, '|');
            int endPinId = std::stoi(token);

            // This works because the nodes are always created before the links, thanks to the serialization order
            Pin* startPin = FindPin(startPinId);
            Pin* endPin = FindPin(endPinId);

            if (startPin != nullptr && endPin != nullptr)
                LinkPins(*startPin, *endPin);
            else
                std::cerr << "The start or the end pin does not exist, skipping\n";
        }
    }
    
    fin.close();

    // Update the unique IDs
    RecountNodeUniqueIds();
    RecountLinkUniqueIds();
}

void ShaderGraphManager::WriteShaderGraphData()
{
    // Write the node data to a separate file
    nodeDataPath = "assets/shader_graph/ShaderGraphData.sgd";
    std::ofstream fout(nodeDataPath);

    for (Node* node : graphNodes)
        fout << "n|" << Node::SerializeNode(node).c_str() << "\n";

    for (const Link* link : graphLinks)
        fout << "l|" << link->startPinID.Get() << "|" << link->endPinID.Get() << "|\n";
    
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
            // std::cout << "Couldn't load node\n";
            return 0;
        }
        
        // std::cout << "Loaded node\n";

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

    // Clear previous data
    uniqueNodeId = 0;
    uniquePinId = 0;
    uniqueLinkId = 0;

    graphNodes.clear();
    graphLinks.clear();
    
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

    for (Node* node : graphNodes)
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
    if (pin.interaction == PinInteraction::Fixed)
    {
        switch (pin.type)
        {
        case PinType::Float:
            DrawFloatSlider(pin);
            ImGui::SameLine();
                
            ImGui::Text("%s", pin.name.c_str());
            
            break;

        default:
            ImGui::Text("%s", pin.name.c_str());
            break;
        }
        
        return;
    }
    
    ed::BeginPin(pin.id, ed::PinKind::Input);
    {
        if (pin.link != nullptr)
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

void ShaderGraphManager::RemoveLink(int linkId)
{
    auto id = std::find_if(graphLinks.begin(), graphLinks.end(),
        [linkId](const Link* link) { return static_cast<int>(link->id.Get()) == linkId; });
    if (id == graphLinks.end())
        return;

    Pin* endPin = FindPin((*id)->endPinID);
    if (endPin != nullptr)
        endPin->link = nullptr;

    graphLinks.erase(id);
    
    RecountLinkUniqueIds();

    WriteShaderGraphData();
}

void ShaderGraphManager::LinkPins(Pin& startPin, Pin& endPin)
{
    if (endPin.link != nullptr)
    {
        // Delete the previous link
        ed::DeleteLink(endPin.link->id);
        RemoveLink(static_cast<int>(endPin.link->id.Get()));
    }

    Link* newLink = new Link(++uniqueLinkId, startPin.id, endPin.id);
    newLink->startPin = FindPin(startPin.id);
    newLink->endPin = FindPin(endPin.id);
    newLink->color = ImColor(1.0f, 1.0f, 1.0f, 1.0f); // GetIconColor(startPin->Type);

    graphLinks.push_back(newLink);
    
    // graphLinks.emplace_back(++uniqueLinkId, startPin.id, endPin.id);
    // graphLinks.back().startPin = FindPin(startPin.id);
    // graphLinks.back().endPin = FindPin(endPin.id);
    // graphLinks.back().color = ImColor(1.0f, 1.0f, 1.0f, 1.0f); // GetIconColor(startPin->Type);

    endPin.link = newLink;

    WriteShaderGraphData();
}

void ShaderGraphManager::DrawLinks()
{
    for (const Link* link : graphLinks)
        ed::Link(link->id, link->startPinID, link->endPinID, link->color, 2.0f);
}

void ShaderGraphManager::QueryLinks()
{
    if (createNewNode)
        return;

    auto currMousePos = ImGui::GetMousePos();

    if (ed::BeginCreate(ImVec4(1, 1, 1, 1), 2))
    {
        auto showLabel = [](const char* label, ImColor color)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
            auto size = ImGui::CalcTextSize(label);

            auto padding = ImGui::GetStyle().FramePadding;
            auto spacing = ImGui::GetStyle().ItemSpacing;

            ImVec2 CursorPos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(CursorPos.x + spacing.x, CursorPos.y - spacing.y));

            ImVec2 CursorScreenPos = ImGui::GetCursorScreenPos();
            auto rectMin = ImVec2(CursorScreenPos.x - padding.x, CursorScreenPos.y - padding.y);
            auto rectMax = ImVec2(CursorScreenPos.x + size.x + padding.x, CursorScreenPos.y + size.y + padding.y);

            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
            ImGui::TextUnformatted(label);
        };
        
        Pin* newLinkPin = nullptr;
        
        ed::PinId startPinId = 0, endPinId = 0;
        if (ed::QueryNewLink(&startPinId, &endPinId))
        {
            Pin* startPin = FindPin(startPinId);
            Pin* endPin = FindPin(endPinId);
        
            newLinkPin = startPin ? startPin : endPin;
        
            if (startPin->kind == PinKind::Input)
            {
                std::swap(startPin, endPin);
                std::swap(startPinId, endPinId);
            }
        
            if (startPin && endPin)
            {
                if (endPin == startPin)
                {
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (endPin->kind == startPin->kind)
                {
                    showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (endPin->node == startPin->node)
                {
                    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                } else if (endPin->type != startPin->type)
                {
                    showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                } else
                {
                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                    if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                    {
                        LinkPins(*startPin, *endPin);
                    }
                }
            }
        }
        
        ed::PinId pinId = 0;
        if (ed::QueryNewNode(&pinId))
        {
            newLinkPin = FindPin(pinId);
            if (newLinkPin)
                showLabel("+ Create Node", ImColor(32, 45, 32, 180));
            
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

                RecountNodeUniqueIds();
            }
        }

        ed::LinkId linkId = 0;
        while (ed::QueryDeletedLink(&linkId))
            if (ed::AcceptDeletedItem())
                RemoveLink(static_cast<int>(linkId.Get()));
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
        
        // // Special
        // ImGui::Separator();
        // if (ImGui::MenuItem("Fresnel"))
        //     node = BuildNode(DummyNode::GetTypeName());

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
    DrawLinks();
    QueryLinks();
    DrawPopups();
}


void ShaderGraphManager::Draw()
{
    auto& io = ImGui::GetIO();

    ShowMenuBar();

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
    std::cout << "Generating shaders..\n";

    // Generate the vertex shader
    auto vertexShaderNode = std::find_if(graphNodes.begin(), graphNodes.end(),
        [](Node* node)
        {
            return dynamic_cast<VertexShaderNode*>(node) != nullptr;
        });
    if (vertexShaderNode != graphNodes.end())
    {
        // Create the code for the vertex shader
        std::unordered_map<std::string, std::string> uniforms;
        std::string vertexShaderCode = (*vertexShaderNode)->GenerateShaderCode(uniforms);

        // Write the code to a file
        std::string vertexShaderPath = PATH_JOIN(FileSystem::rootDirectory, ENGINE_PATH::ASSETS,
            "Shaders\\ShaderGraphGen\\TestShaderGraph.VS.glsl");
        
        std::ofstream fout(vertexShaderPath);
        fout << vertexShaderCode;

        fout.close();
    }

    // Generate the fragment shader
    auto fragmentShaderNode = std::find_if(graphNodes.begin(), graphNodes.end(),
        [](Node* node)
        {
            return dynamic_cast<FragmentShaderNode*>(node) != nullptr;
        });
    if (fragmentShaderNode != graphNodes.end())
    {
        // Create the code for the fragment shader
        std::unordered_map<std::string, std::string> uniforms;
        std::string fragmentShaderCode = (*fragmentShaderNode)->GenerateShaderCode(uniforms);

        // Write the code to a file
        std::string fragmentShaderPath = PATH_JOIN(FileSystem::rootDirectory, ENGINE_PATH::ASSETS,
            "Shaders\\ShaderGraphGen\\TestShaderGraph.FS.glsl");
        
        std::ofstream fout(fragmentShaderPath);
        fout << fragmentShaderCode;
        
        fout.close();
    }

    GUIManager::GetInstance()->MarkReloadShaders();

    std::cout << "Shader has been generated!\n";
}
