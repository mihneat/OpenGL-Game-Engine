#pragma once
#include <map>
#include <string>
#include <unordered_map>

typedef std::string Guid;

struct SerializedObject
{
    Guid guid;
    std::string name;
    std::string type;
    void* ptr = nullptr;
};

class Database
{
public:
    static void AddEntry(const SerializedObject& obj);

    static SerializedObject GetByPtr(void* ptr);
    static SerializedObject GetByGUID(const Guid& guid);
    static const std::map<std::string, SerializedObject>& GetByType(const std::string& type);

private:
    static std::unordered_map<void*, SerializedObject> ptrToObj;
    static std::unordered_map<Guid, SerializedObject> guidToObj;
    static std::unordered_map<std::string, std::map<std::string, SerializedObject>> typeToObjs;
    
};
