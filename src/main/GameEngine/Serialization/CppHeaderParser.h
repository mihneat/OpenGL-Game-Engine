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
    FieldTypeEnum,
    FieldTypeTransform,
    FieldTypeGUID,
    FieldTypeUnimplemented,
};

struct SerializedField
{
    std::string name;
    FieldType type;
    std::string enumName = "";

    std::string GetTypeName() const;
    static FieldType GetFromTypeName(const std::string& str);
    
};

struct ClassInfo
{
    std::string classPath;
    std::string className;
    std::string classParent;
    std::vector<SerializedField> serializedFields;
};

struct EnumInfo
{
    std::string enumName;
    std::vector<std::string> values;
};

typedef std::map<std::string, ClassInfo> SerializationResult;

class CppHeaderParser
{
public:
    CppHeaderParser() = default;
    ~CppHeaderParser() = default;

    static void GenerateSerializedData();
};
