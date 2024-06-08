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
    FieldTypeString,
    FieldTypeTransform,
    FieldTypeUnimplemented,
};

struct SerializedField
{
    std::string name;
    FieldType type;

    std::string GetTypeName() const;
};

struct ClassInfo
{
    std::string classPath;
    std::string className;
    std::string classParent;
    std::vector<SerializedField> serializedFields;
};

typedef std::map<std::string, ClassInfo> SerializationResult;

class CppHeaderParser
{
public:
    CppHeaderParser() = default;
    ~CppHeaderParser() = default;

    static void GenerateSerializedData();
};
