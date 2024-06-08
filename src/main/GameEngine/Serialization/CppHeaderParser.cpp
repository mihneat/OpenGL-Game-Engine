#include "CppHeaderParser.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "Serializer.h"
#include "core/managers/resource_path.h"

std::pair<std::string, std::string> GetClassAndParentName(std::ifstream& fin, const std::string& remainder)
{
    // Form the entire class declaration
    std::string classDeclaration;
    if (remainder.find('{') == std::string::npos)
    {
        // Get lines until an open bracket is found, or the file ends
        std::string line;
        std::getline(fin, line, '{');
        classDeclaration.append(remainder).append(line);
    } else
    {
        // Cut the declaration at the '{'
        std::stringstream stringStream(remainder);
        std::getline(stringStream, classDeclaration, '{');
    }

    std::string className;
    std::string parentName;

    bool awaitClassName = false;
    bool foundClassName = false;
    std::string word;
    std::stringstream stringStream(classDeclaration);
    while (std::getline(stringStream, word, ' '))
    {
        if (word.empty())
            continue;

        if (foundClassName)
        {
            if (word == ":")
                continue;
            
            if (word == "public" || word == "protected" || word == "private")
                continue;

            // Found the parent name, but it might have a comma
            // I really have to move to a C++ parser
            std::stringstream strStream(word[0] == ':' ? word.erase(0, 1) : word);
            std::getline(strStream, word, ',');
            
            std::stringstream strStreamNoNewline(word);
            std::getline(strStreamNoNewline, word, '\n');

            parentName = word;
            return {className, parentName};
        }

        if (!foundClassName && awaitClassName)
        {
            // Check if the word has a colon
            std::stringstream strStream(word);
            std::getline(strStream, word, ':');
        
            className = word;
            foundClassName = true;

            // Try to get the parent name
            while (std::getline(strStream, word, ':'))
            {
                if (word.empty())
                    continue;

                if (word == "public" || word == "protected" || word == "private")
                    continue;

                parentName = word;
                return {className, parentName};
            }
            
            continue;
        }
        
        if (!foundClassName && word == "class")
            awaitClassName = true;
    }

    if (className.empty())
        return {};

    return {className, parentName};
}

SerializedField GetSerializedField(std::ifstream& fin, const std::string& remainder)
{
    // Form the entire statement
    std::string statement;
    if (remainder.find(';') == std::string::npos)
    {
        // Get lines until a semicolon is found, or the file ends
        std::string line;
        std::getline(fin, line, ';');
        statement.append(remainder).append(line);
    } else
    {
        // Cut the statement at the ';'
        std::stringstream stringStream(remainder);
        std::getline(stringStream, statement, ';');
    }

    // Get everything before the initialization
    std::string fieldDeclaration;
    std::stringstream stringStream(statement);
    std::getline(stringStream, fieldDeclaration, '=');

    // std::cout << "Field declaration: " << fieldDeclaration << "\n";

    // Extract all the words into a vector
    std::vector<std::string> words;
    std::string word;

    std::stringstream stringStreamDecl(fieldDeclaration);
    while (std::getline(stringStreamDecl, word, ' '))
    {
        if (word.empty())
            continue;

        words.push_back(word);
    }

    // The last word is the attribute name
    std::string attributeName = words.size() > 1 ? words[words.size() - 1] : "";

    // Try to find the attribute type
    bool isPointer = fieldDeclaration.find('*') != std::string::npos;
    FieldType type = FieldTypeUnimplemented;
    for (size_t i = 0; i < words.size() - 1; ++i)
    {
        const std::string& currWord = words[i];
        
        // TODO: Place HERE a "generic" Component type class

        if (currWord.find("unsigned") != std::string::npos)
            break;
        
        if (currWord.find("const") != std::string::npos)
            break;

        if (currWord.find("bool") != std::string::npos)
        {
            if (isPointer)
                break;

            type = FieldTypeBool;
            
            break;
        }
        
        if (currWord.find("int") != std::string::npos)
        {
            if (isPointer)
                break;

            type = FieldTypeInt;
            
            break;
        }
        
        if (currWord.find("float") != std::string::npos)
        {
            if (isPointer)
                break;

            type = FieldTypeFloat;
            
            break;
        }
        
        if (currWord.find("glm::vec2") != std::string::npos)
        {
            if (isPointer)
                break;

            type = FieldTypeVec2;
            
            break;
        }
        
        if (currWord.find("glm::vec3") != std::string::npos)
        {
            if (isPointer)
                break;

            type = FieldTypeVec3;
            
            break;
        }
        
        if (currWord.find("glm::vec4") != std::string::npos)
        {
            if (isPointer)
                break;

            type = FieldTypeColour;
            
            break;
        }
        
        if (currWord.find("std::string") != std::string::npos)
        {
            if (isPointer)
                break;

            type = FieldTypeString;
            
            break;
        }
        
        if (currWord.find("Transform") != std::string::npos)
        {
            if (!isPointer)
                break;

            type = FieldTypeTransform;
            
            break;
        }
    }
    
    return {attributeName, type};
}

ClassInfo ParseHeaderFile(const std::filesystem::path& headerPath)
{
    // Manually exclude classes
    if (headerPath.filename() == "Component.h")
    {
        std::cout << "Excluding: " << headerPath.filename() << "...\n";
        return {};
    }
    
    std::cout << "Parsing: " << headerPath.filename() << "...\n";

    static const std::string classSerializationMacro("SERIALIZE_CLASS");
    static const std::string attributeSerializationMacro("SERIALIZE_FIELD");

    bool isClassSerializable = false;
    std::string className;
    std::string classParent;
    std::vector<SerializedField> serializedFields;
    
    std::ifstream fin(headerPath);

    // Read the file line by line
    std::string line;
    while (std::getline(fin, line))
    {
        // Check for the class serialization macro
        size_t pos = line.find(classSerializationMacro);
        if (pos != std::string::npos)
        {
            // Check if it's the second declaration
            if (isClassSerializable)
            {
                std::cerr << "Header " << headerPath.filename() << " has more than one " <<
                    classSerializationMacro << ", ignoring the file..\n";
                return {};
            }

            isClassSerializable = true;

            // Search for the class name in the remainder of the line
            std::string remainder = line.erase(0, pos + classSerializationMacro.length());
            std::pair<std::string, std::string> res = GetClassAndParentName(fin, remainder);
            if (res.first.empty())
            {
                std::cerr << "Class serialization macro must be placed right before a class definition\n";
                return {};
            }

            // The class name was found, continue the search
            className = res.first;
            classParent = res.second;

            continue;
        }

        pos = line.find(attributeSerializationMacro);
        if (pos != std::string::npos && isClassSerializable)
        {
            // Search for the attribute info in the remainder of the line
            std::string remainder = line.erase(0, pos + attributeSerializationMacro.length());
            SerializedField res = GetSerializedField(fin, remainder);
            if (res.name.empty())
            {
                std::cerr << "Field serialization macro must be placed right before a field declaration\n";
                return {};
            }

            if (res.type == FieldTypeUnimplemented)
            {
                std::cerr << "Type of field " << res.name << " is unimplemented\n";
                return {};
            }

            // A new serialized field was found
            serializedFields.push_back(res);

            continue;
        }
    }
    
    if (isClassSerializable)
    {
        std::cout << "Class " << className << " with parent " << classParent << " is serializable!\n";
        std::cout << "Serialized fields:\n";
        for (auto& it : serializedFields)
        {
            std::cout << "  * " << it.name << ": " << it.GetTypeName() << "\n";
        }

        std::cout << "\n";
        
        return {headerPath.string(), className, classParent, serializedFields};
    }
    
    return {};
}

void GenerateSerializer(const SerializationResult& s)
{
    static const std::string modelPath = PATH_JOIN(ENGINE_PATH::GAME_ENGINE, "Serialization", "Serializer.model");
    static const std::string serializerPath = PATH_JOIN(ENGINE_PATH::GAME_ENGINE, "Serialization", "Serializer.cpp");
    
    std::string includeTemplate;
    std::string serializedClassFieldsTemplate;
    std::string attributeResolverTemplate;
    
    for (auto& it : s)
    {
        // Step #1 - Form the include header
        std::string includeHeader(R"(#include "..\)");
        includeHeader.append(it.second.classPath)
            .append("\"\n");

        includeTemplate.append(includeHeader);
        
        // Step #2 - Form the serialized class fields
        std::string serializedClassFields("        {\"");
        serializedClassFields.append(it.second.className)
            .append(R"(", std::vector<SerializedField>{)");

        for (auto& serializedField : it.second.serializedFields)
        {
            serializedClassFields.append("{\"")
                .append(serializedField.name)
                .append("\", ")
                .append(serializedField.GetTypeName())
                .append("},");
        }

        serializedClassFields.append("}},\n");
        serializedClassFieldsTemplate.append(serializedClassFields);

        // Step 3 - Form the attribute resolver
        std::string attributeResolver("    if (dynamic_cast<");
        attributeResolver.append(it.second.className).append("*>(instance) != nullptr)\n")
            .append("    {\n")
            .append("        ").append(it.second.className).append("* obj = dynamic_cast<").append(it.second.className).append("*>(instance);\n\n");

        for (auto& serializedField : it.second.serializedFields)
        {
            attributeResolver.append("        if (attributeName == \"").append(serializedField.name).append("\")\n")
                             .append("            return &obj->").append(serializedField.name).append(";\n\n");
        }

        attributeResolver.append("        return nullptr;\n")
            .append("    }\n\n");
        
        /**
         * Template(CLASS_NAME, ATTRIBUTE_NAME[]):
         *
         * if (dynamic_cast<|CLASS_NAME|*>(instance) != nullptr)
         * {
         *      |CLASS_NAME|* obj = dynamic_cast<|CLASS_NAME|*>(instance);
         *
         *      ||FOR_EACH ATTRIBUTE_NAME||
         *      if (attributeName == "|ATTRIBUTE_NAME|")
         *          return &obj->|ATTRIBUTE_NAME|;
         *
         *      ||END_FOR_EACH||
         *      return nullptr;
         * }
         *
         */
        attributeResolverTemplate.append(attributeResolver);
    }

    // Step 4 - Form the final file
    const std::unordered_map<std::string, std::string> templates = {
        {"{{INCLUDE_HEADERS}}", includeTemplate},
        {"{{SERIALIZED_CLASS_FIELDS}}", serializedClassFieldsTemplate},
        {"{{ATTRIBUTE_RESOLVER}}", attributeResolverTemplate},
    };
    
    std::ifstream fin(modelPath);
    std::ofstream fout(serializerPath);
    
    std::string line;
    while (std::getline(fin, line))
    {
        // !! Limitation: One key per line
        // Check if the line contains any key
        for (auto& it : templates)
        {
            size_t pos = line.find(it.first);
            if (pos == std::string::npos)
                continue;

            line.erase(pos, it.first.length());
            line.insert(pos, it.second);
        }
        
        fout << line << "\n";
    }
}

void CppHeaderParser::GenerateSerializedData()
{
    std::cout << "Header file parsing started!\n";

    SerializationResult result;

    // Inspired from: https://stackoverflow.com/a/23658737
    for (std::filesystem::recursive_directory_iterator file(ENGINE_PATH::GAME_ENGINE), end; file != end; ++file)
    {
        // Ignore directories
        if (is_directory(file->path()))
            continue;

        // Ignore files with no extension
        if (!file->path().has_extension())
            continue;

        // Ignore files that are not .h or .hpp
        if (file->path().extension().compare(".h") && file->path().extension().compare(".hpp"))
            continue;

        // Parse the file
        const ClassInfo classInfo = ParseHeaderFile(file->path());
        if (classInfo.className.empty())
            continue;
        
        result[classInfo.className] = classInfo;
    }

    // Resolve parent serialization
    for (auto& it : result)
    {
        // Go through all the parents until reaching "Component", no parent, or a non-serialized parent
        ClassInfo* curr = &it.second;
        while (true)
        {
            const std::string& parent = curr->classParent;
            if (parent.empty())
                break;

            // Only for safety
            if (it.second.className == parent)
                break;
            
            if (parent == "Component")
                break;

            if (result.find(parent) == result.end())
                break;

            // Add the parent's serialized fields, in reverse order
            ClassInfo& parentClassInfo = result[parent];
            for (int i = (int)parentClassInfo.serializedFields.size() - 1; i >= 0; --i)
            {
                curr->serializedFields.insert(curr->serializedFields.begin(), parentClassInfo.serializedFields[i]);
            }

            curr = &result[parent];
        }
    }
    
    std::cout << "Header file iteration complete\n";

    // Generate the Serializer file
    // TODO: Remove hardcode
    // const ClassInfo obstacleClassInfo = {
    //     R"(src\main\GameEngine\ComponentBase\Components\Logic\Objects\Obstacle.h)",
    //     "Obstacle",
    //     std::vector<SerializedField>{
    //                 { "collisionRadius", FieldTypeFloat },
    //                 { "isHazard", FieldTypeBool }
    //     }
    // };
    // result[obstacleClassInfo.className] = obstacleClassInfo;
    //
    // const ClassInfo groundStickClassInfo = {
    //     R"(src\main\GameEngine\ComponentBase\Components\Logic\Player\GroundStick.h)",
    //     "GroundStick",
    //     std::vector<SerializedField>{}
    // };
    // result[groundStickClassInfo.className] = groundStickClassInfo;

    GenerateSerializer(result);
    std::cout << "Generated new Serialize.cpp\n";
    
    std::cout << "Header file parsing finished!\n";
}

std::string SerializedField::GetTypeName() const
{
    switch (type)
    {
    case FieldTypeBool:
        return "FieldTypeBool";
        
    case FieldTypeInt:
        return "FieldTypeInt";
        
    case FieldTypeFloat:
        return "FieldTypeFloat";
        
    case FieldTypeVec2:
        return "FieldTypeVec2";
        
    case FieldTypeVec3:
        return "FieldTypeVec3";
        
    case FieldTypeColour:
        return "FieldTypeColour";
        
    case FieldTypeString:
        return "FieldTypeString";
        
    case FieldTypeTransform:
        return "FieldTypeTransform";

    case FieldTypeUnimplemented:
    default:
        std::cerr << "[CppHeaderParser] Field type is unimplemented!!\n";
        return "FieldTypeUnimplemented";
    }
}
