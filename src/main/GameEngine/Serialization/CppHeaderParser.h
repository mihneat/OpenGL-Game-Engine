#pragma once
#include <map>
#include <string>
#include <vector>

enum FieldType
{
    FieldTypeBool,
    FieldTypeInt,
    FieldTypeFloat,
    FieldTypeVec2,
    FieldTypeVec3,
    FieldTypeColour,
    FieldTypeTransform,
};

struct SerializedField
{
    std::string name;
    FieldType type;
};

typedef std::map<std::string, std::vector<SerializedField>> SerializationResult;

class CppHeaderParser
{
public:
    CppHeaderParser() = default;
    ~CppHeaderParser() = default;

    static void GenerateSerializedData();
};
