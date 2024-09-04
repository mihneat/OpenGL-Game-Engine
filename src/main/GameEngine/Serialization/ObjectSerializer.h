#pragma once
#include <nlohmann/json.hpp>

#include "main/GameEngine/ComponentBase/Transform.h"

class ObjectSerializer
{
public:
    static transform::Transform* DeserializeRootObject(const nlohmann::ordered_json& object);
    static nlohmann::ordered_json SerializeRootObject(const transform::Transform* root);

private:
    static transform::Transform* DeserializeTransform(const nlohmann::ordered_json& transformObj, transform::Transform* parent);
    static std::vector<transform::Transform*> DeserializeTransformArray(const nlohmann::ordered_json& transformsObj, transform::Transform* parent);

    static bool hasSetGameInstance;
    
};
