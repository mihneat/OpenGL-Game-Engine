#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/PointLight.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/Managers/LightManager.h"

#include <iostream>

using namespace std;
using namespace loaders;
using namespace component;
using namespace transform;

PointLight::PointLight(transform::Transform* transform) : Light(transform, LightManager::Point) { }
