#include "Database.h"

std::unordered_map<void*, SerializedObject> Database::ptrToObj = {{nullptr, {}}};
std::unordered_map<Guid, SerializedObject> Database::guidToObj = {{"", {}}};
std::unordered_map<std::string, std::map<std::string, SerializedObject>> Database::typeToObjs = {{"", {}}};

void Database::AddEntry(const SerializedObject& obj)
{
    ptrToObj[obj.ptr] = obj;
    guidToObj[obj.guid] = obj;
    typeToObjs[obj.type][obj.name] = obj;
}

SerializedObject Database::GetByPtr(void* ptr)
{
    if (ptrToObj.find(ptr) == ptrToObj.end())
        return {};
    
    return ptrToObj[ptr];
}

SerializedObject Database::GetByGUID(const Guid& guid)
{
    if (guidToObj.find(guid) == guidToObj.end())
        return {};
    
    return guidToObj[guid];
}

const std::map<std::string, SerializedObject>& Database::GetByType(const std::string& type)
{
    static const std::map<std::string, SerializedObject> emptyObjs = {};

    if (typeToObjs.find(type) == typeToObjs.end())
        return emptyObjs;
    
    return typeToObjs[type];
}
