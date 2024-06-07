#pragma once
#include <string>
#include <vector>

#include "main/GameEngine/ComponentBase/Component.h"

struct SerializedField;

class Serializer
{
public:
    static const std::vector<SerializedField>& GetSerializedFieldsForClass(const std::string& className);
    static void* GetAttributeReference(component::Component* instance, const std::string& attributeName);
};
