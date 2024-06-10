#pragma once
#include <nlohmann/json.hpp>

#include "main/GameEngine/ComponentBase/Transform.h"

class ObjectSerializer
{
public:
    static transform::Transform* DeserializeRootObject(const nlohmann::json& object);
};
