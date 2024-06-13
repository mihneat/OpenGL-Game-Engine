#include "ObjectSerializer.h"

#include <iostream>

#include "CppHeaderParser.h"
#include "Database.h"
#include "Serializer.h"
#include "main/GameEngine/ComponentBase/Component.h"
#include "main/GameEngine/Managers/GameInstance.h"
#include "main/GameEngine/Utils/Utils.h"
#include "main/GameEngine/Systems/Rendering/Texture.h"

using namespace std;
using namespace transform;
using namespace component;
using ordered_json = nlohmann::ordered_json;

bool ObjectSerializer::hasSetGameInstance = false;

ordered_json SerializeTransformArray(const Transform* transformsParent);

bool DeserializeBool(const ordered_json& boolObj)
{
    return boolObj;
}

int DeserializeInt(const ordered_json& intObj)
{
    return intObj;
}

float DeserializeFloat(const ordered_json& floatObj)
{
    return floatObj;
}

glm::vec2 DeserializeVec2(const ordered_json& vec2Obj)
{
    return {
        vec2Obj["x"],
        vec2Obj["y"]
    };
}

glm::vec3 DeserializeVec3(const ordered_json& vec3Obj)
{
    return {
        vec3Obj["x"],
        vec3Obj["y"],
        vec3Obj["z"]
    };
}

glm::vec4 DeserializeColor(const ordered_json& colorObj)
{
    return {
        colorObj["r"],
        colorObj["g"],
        colorObj["b"],
        colorObj["a"]
    };
}

string DeserializeString(const ordered_json& stringObj)
{
    return stringObj;
}

void* DeserializeGUID(const ordered_json& guidObj)
{
    const SerializedObject& obj = Database::GetByGUID(guidObj["guid"]);

    return obj.ptr;
}

void DeserializeAttribute(const ordered_json& attributeObj, Component* instance)
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

    // TODO: Does this work?
    case FieldTypeEnum:
        *static_cast<int*>(data) = DeserializeInt(attributeObj["value"]);
        break;

    // TODO: This does NOT work with regular serialization :(
    case FieldTypeTransform:
        *static_cast<Transform**>(data) = nullptr;
        break;
        
    case FieldTypeGUID:
        if (static_cast<SerializableObject**>(data) != nullptr)
        {
            *static_cast<SerializableObject**>(data) = static_cast<SerializableObject*>(DeserializeGUID(attributeObj["value"]));
            break;
        }
        
        std::cerr << "Pointer type is not serializable\n";
        break;
        
    case FieldTypeUnimplemented:
        std::cerr << "[SceneManager] Type not implemented!\n";
        break;
    }
}

void DeserializeAttributeArray(const ordered_json& attributesObj, Component* instance)
{
    for (auto& attributeObj : attributesObj)
        DeserializeAttribute(attributeObj, instance);
}

Component* DeserializeComponent(const ordered_json& componentObj, Transform* parent)
{
    Component* component = Serializer::ComponentFactory(DeserializeString(componentObj["className"]), parent);
    component->SetActive(DeserializeBool(componentObj["enabled"]));

    // Set all the attributes
    DeserializeAttributeArray(componentObj["attributes"], component);
    
    return component;
}

std::vector<Component*> DeserializeComponentArray(const ordered_json& componentsObj, Transform* parent)
{
    std::vector<Component*> components;
    for (auto& componentObj : componentsObj)
        components.push_back(DeserializeComponent(componentObj, parent));

    return components;
}

Transform* ObjectSerializer::DeserializeTransform(const ordered_json& transformObj, Transform* parent)
{
    Transform* transform = new Transform(
        parent,
        DeserializeString(transformObj["name"]),
        DeserializeString(transformObj["tag"])
    );
    transform->Translate(DeserializeVec3(transformObj["position"]));
    transform->Rotate(DeserializeVec3(transformObj["rotation"]));
    transform->SetScale(DeserializeVec3(transformObj["scale"]));

    // Add the components
    for (const auto it : DeserializeComponentArray(transformObj["components"], transform))
        transform->AddComponent(it);
    
    // Create the children
    DeserializeTransformArray(transformObj["children"], transform);

    return transform;
}

std::vector<Transform*> ObjectSerializer::DeserializeTransformArray(const ordered_json& transformsObj, Transform* parent)
{
    std::vector<Transform*> transforms;
    
    for (auto& transformObj : transformsObj)
    {
        // TODO: Temporary until the GameInstance functionality is fully implemented
        Transform* transform = DeserializeTransform(transformObj, parent);
        if (!hasSetGameInstance)
        {
            managers::GameInstance::gameInstance->AttachTransform(transform);
            hasSetGameInstance = true;
        }
        
        transforms.push_back(transform);
    }

    return transforms;
}

Transform* ObjectSerializer::DeserializeRootObject(const ordered_json& object)
{
    Transform* root = new Transform();

    // Create the root object
    hasSetGameInstance = false;
    DeserializeTransformArray(object["root"], root);

    return root;
}

ordered_json SerializeBool(bool tBool)
{
    return tBool;
}

ordered_json SerializeInt(int tInt)
{
    return tInt;
}

ordered_json SerializeFloat(float tFloat)
{
    return tFloat;
}

ordered_json SerializeVec2(glm::vec2 tVec2)
{
    ordered_json vec2Obj;
    vec2Obj["x"] = tVec2.x;
    vec2Obj["y"] = tVec2.y;

    return vec2Obj;
}

ordered_json SerializeVec3(glm::vec3 tVec3)
{
    ordered_json vec3Obj;
    vec3Obj["x"] = tVec3.x;
    vec3Obj["y"] = tVec3.y;
    vec3Obj["z"] = tVec3.z;

    return vec3Obj;
}

ordered_json SerializeColor(glm::vec4 tColor)
{
    ordered_json colorObj;
    colorObj["r"] = tColor.r;
    colorObj["g"] = tColor.g;
    colorObj["b"] = tColor.b;
    colorObj["a"] = tColor.a;

    return colorObj;
}

ordered_json SerializeString(string tString)
{
    return tString;
}

ordered_json SerializeGUID(void* ptr)
{
    ordered_json guidObj;

    const SerializedObject& obj = Database::GetByPtr(ptr);
    guidObj["guid"] = obj.guid;

    return guidObj;
}

ordered_json SerializeAttribute(Component* instance, const SerializedField& attribute)
{
    ordered_json attributeObj;

    attributeObj["name"] = SerializeString(attribute.name);
    attributeObj["type"] = SerializeString(attribute.GetTypeName());
    
    const FieldType type = attribute.type;
    void* data = Serializer::GetAttributeReference(instance, attribute.name);
    
    switch (type)
    {
    case FieldTypeBool:
        attributeObj["value"] = SerializeBool(*static_cast<bool*>(data));
        break;
        
    case FieldTypeInt:
        attributeObj["value"] = SerializeInt(*static_cast<int*>(data));
        break;
        
    case FieldTypeFloat:
        attributeObj["value"] = SerializeFloat(*static_cast<float*>(data));
        break;
        
    case FieldTypeVec2:
        attributeObj["value"] = SerializeVec2(*static_cast<glm::vec2*>(data));
        break;
        
    case FieldTypeVec3:
        attributeObj["value"] = SerializeVec3(*static_cast<glm::vec3*>(data));
        break;
        
    case FieldTypeColour:
        attributeObj["value"] = SerializeColor(*static_cast<glm::vec4*>(data));
        break;
        
    case FieldTypeString:
        attributeObj["value"] = SerializeString(*static_cast<std::string*>(data));
        break;
        
    case FieldTypeEnum:
        attributeObj["value"] = SerializeInt(*static_cast<int*>(data));
        break;

        // TODO: This will NOT work with regular serialization :(
    case FieldTypeTransform:
        attributeObj["value"] = SerializeString("<unimplemented>");
        break;
        
    case FieldTypeGUID:
        if (static_cast<SerializableObject**>(data) != nullptr)
        {
            attributeObj["value"] = SerializeGUID(*static_cast<SerializableObject**>(data));
            break;
        }
        std::cerr << "Pointer type is not serializable\n";
        
        return ordered_json::object();
        
    case FieldTypeUnimplemented:
        std::cerr << "[SceneManager] Type not implemented!\n";
        return ordered_json::object();
    }

    return attributeObj;
}

ordered_json SerializeAttributeArray(Component* component, const std::string& className)
{
    ordered_json attributesObj = ordered_json::array();

    for (const auto& attribute : Serializer::GetSerializedFieldsForClass(className))
    {
        ordered_json attributeObj = SerializeAttribute(component, attribute);
        if (attributeObj != ordered_json::object())
            attributesObj.push_back(attributeObj);
    }

    return attributesObj;
}

ordered_json SerializeComponent(Component* component)
{
    ordered_json componentObj = ordered_json::object();

    const std::string className = Utils::ExtractClassName(typeid(*component).name());
    componentObj["className"] = SerializeString(className);
    componentObj["enabled"] = SerializeBool(component->IsActive());
    componentObj["attributes"] = SerializeAttributeArray(component, className);

    return componentObj;
}

ordered_json SerializeComponentArray(const Transform* transform)
{
    ordered_json componentsObj = ordered_json::array();

    for (int i = 0; i < transform->GetComponentCount(); ++i)
        componentsObj.push_back(SerializeComponent(transform->GetComponentByIndex(i)));

    return componentsObj;
}

ordered_json SerializeTransform(const Transform* transform)
{
    ordered_json transformObj = ordered_json::object();

    transformObj["name"] = SerializeString(transform->GetName());
    transformObj["tag"] = SerializeString(transform->GetTag());

    transformObj["position"] = SerializeVec3(transform->GetLocalPosition());
    transformObj["rotation"] = SerializeVec3(transform->GetLocalRotation());
    transformObj["scale"] = SerializeVec3(transform->GetLocalScale());

    transformObj["components"] = SerializeComponentArray(transform);
    
    transformObj["children"] = SerializeTransformArray(transform);

    return transformObj;
}

ordered_json SerializeTransformArray(const Transform* transformsParent)
{
    ordered_json transformsObj = ordered_json::array();

    for (int i = 0; i < transformsParent->GetChildCount(); ++i)
        transformsObj.push_back(SerializeTransform(transformsParent->GetChild(i)));

    return transformsObj;
}

ordered_json ObjectSerializer::SerializeRootObject(const Transform* root)
{
    ordered_json object = ordered_json::object();

    // Create the root object
    object["root"] = SerializeTransformArray(root);

    return object;
}
