#pragma once
#include <string>
#include <vector>

#include "main/GameEngine/ComponentBase/Component.h"

struct SerializedField;

class Serializer
{
public:
    static const std::vector<SerializedField>& GetSerializedFieldsForClass(const std::string& className);
    // TODO: Replace "return nullptr" with exception throwing
    static void* GetAttributeReference(component::Component* instance, const std::string& attributeName);
    static component::Component* ComponentFactory(const std::string& className, transform::Transform* parent);
    static const std::vector<std::string>& GetSerializedClasses();
    // TODO: Allow initializations of enum values
    static const std::vector<std::pair<std::string, int>>& GetValuePairsForEnum(const std::string& enumName);
};
