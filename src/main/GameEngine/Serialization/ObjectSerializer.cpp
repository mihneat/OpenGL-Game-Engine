#include "ObjectSerializer.h"

#include <iostream>

#include "CppHeaderParser.h"
#include "Serializer.h"
#include "main/GameEngine/ComponentBase/Component.h"

using namespace std;
using namespace transform;
using namespace component;
using json = nlohmann::json;

std::vector<Transform*> DeserializeTransformArray(const json& transformsObj, Transform* parent);

bool DeserializeBool(const json& boolObj)
{
    return boolObj;
}

int DeserializeInt(const json& intObj)
{
    return intObj;
}

float DeserializeFloat(const json& floatObj)
{
    return floatObj;
}

glm::vec2 DeserializeVec2(const json& vec2Obj)
{
    return {
        vec2Obj["x"],
        vec2Obj["y"]
    };
}

glm::vec3 DeserializeVec3(const json& vec3Obj)
{
    return {
        vec3Obj["x"],
        vec3Obj["y"],
        vec3Obj["z"]
    };
}

glm::vec4 DeserializeColor(const json& colorObj)
{
    return {
        colorObj["r"],
        colorObj["g"],
        colorObj["b"],
        colorObj["a"]
    };
}

string DeserializeString(const json& stringObj)
{
    return stringObj;
}

void DeserializeAttribute(const json& attributeObj, Component* instance)
{
    const FieldType type = SerializedField::GetFromTypeName(DeserializeString(attributeObj["type"]));
    void* data = Serializer::GetAttributeReference(instance, DeserializeString(attributeObj["name"]));

    switch (type)
    {
    case FieldTypeBool:
        *static_cast<bool*>(data) = DeserializeBool(attributeObj["value"]);
        break;
        
    case FieldTypeInt:
        *static_cast<int*>(data) = DeserializeInt(attributeObj["value"]);
        break;
        
    case FieldTypeFloat:
        *static_cast<float*>(data) = DeserializeFloat(attributeObj["value"]);
        break;
        
    case FieldTypeVec2:
        *static_cast<glm::vec2*>(data) = DeserializeVec2(attributeObj["value"]);
        break;
        
    case FieldTypeVec3:
        *static_cast<glm::vec3*>(data) = DeserializeVec3(attributeObj["value"]);
        break;
        
    case FieldTypeColour:
        *static_cast<glm::vec4*>(data) = DeserializeColor(attributeObj["value"]);
        break;
        
    case FieldTypeString:
        *static_cast<std::string*>(data) = DeserializeString(attributeObj["value"]);
        break;

    // TODO: This will NOT work with regular serialization :(
    case FieldTypeTransform:
        *static_cast<Transform*>(data) = nullptr;
        break;
        
    case FieldTypeUnimplemented:
        std::cerr << "[SceneManager] Type not implemented!\n";
        break;
    }
}

void DeserializeAttributes(const json& attributesObj, Component* instance)
{
    for (auto& attributeObj : attributesObj)
        continue; // DeserializeAttribute(attributeObj, instance);
}

Component* DeserializeComponent(const json& componentObj, Transform* parent)
{
    Component* component = new Component(parent); // Serializer::ComponentFactory(DeserializeString(componentObj["className"]), parent);
    component->SetActive(DeserializeBool(componentObj["enabled"]));

    // Set all the attributes
    DeserializeAttributes(componentObj["attributes"], component);
    
    return component;
}

std::vector<Component*> DeserializeComponentArray(const json& componentsObj, Transform* parent)
{
    std::vector<Component*> components;
    for (auto& componentObj : componentsObj)
        components.push_back(DeserializeComponent(componentObj, parent));

    return components;
}

Transform* DeserializeTransform(const json& transformObj, Transform* parent)
{
    Transform* transform = new Transform(
        parent,
        DeserializeString(transformObj["name"]),
        DeserializeString(transformObj["tag"]),
        // TODO: Deprecate manual rotation :(
        DeserializeBool(transformObj["manualRotation"])
    );
    transform->Translate(DeserializeVec3(transformObj["position"]));
    transform->Rotate(DeserializeVec3(transformObj["rotation"]));
    transform->Scale(DeserializeVec3(transformObj["scale"]));

    // Add the components
    for (const auto it : DeserializeComponentArray(transformObj["components"], transform))
        transform->AddComponent(it);
    
    // Add the children
    for (const auto it : DeserializeTransformArray(transformObj["children"], transform))
        transform->AddChild(it);

    return transform;
}

std::vector<Transform*> DeserializeTransformArray(const json& transformsObj, Transform* parent)
{
    std::vector<Transform*> transforms;
    for (auto& transformObj : transformsObj)
        transforms.push_back(DeserializeTransform(transformObj, parent));

    return transforms;
}

Transform* ObjectSerializer::DeserializeRootObject(const json& object)
{
    Transform* root = new Transform();
    for (const auto it : DeserializeTransformArray(object["root"], root))
        root->AddChild(it);

    return root;
}
