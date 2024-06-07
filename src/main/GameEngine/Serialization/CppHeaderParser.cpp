#include "CppHeaderParser.h"

#include <iostream>
#include <filesystem>

#include "Serializer.h"

struct ClassInfo
{
    std::string className;
    std::vector<SerializedField> serializedFields;
};

void CppHeaderParser::GenerateSerializedData()
{
    std::cout << "Header file parsing started!\n";

    SerializationResult result;

    for (std::filesystem::recursive_directory_iterator i("."), end; i != end; ++i) 
        if (!is_directory(i->path()))
            std::cout << i->path().filename() << "\n";
    
    std::cout << "Header file visitor done\n";

    // TODO: Remove hardcode of Obstacle class
    const ClassInfo obstacleClassInfo = {
        "Obstacle",
        std::vector<SerializedField>{
            { "collisionRadius", FieldTypeFloat },
            { "isHazard", FieldTypeBool }
        }
    };
    
    // TODO: Fill in the result with the class info
    result[obstacleClassInfo.className] = obstacleClassInfo.serializedFields;
    std::cout << "Results added to parser\n";

    // TODO: Generate the Serializer file
    
    std::cout << "Header file parsing finished!\n";
}

ClassInfo ParseHeaderFile(/* TODO: Either file path, or, preferably, file handle, depending on file visitor implementation */)
{
    return {};
}
