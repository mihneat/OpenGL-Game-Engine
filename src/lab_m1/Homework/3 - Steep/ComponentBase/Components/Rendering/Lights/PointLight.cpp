#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Rendering/Lights/PointLight.h"
#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "lab_m1/Homework/3 - Steep/Managers/LightManager.h"

#include <iostream>

using namespace std;
using namespace loaders;
using namespace component;
using namespace transform;

PointLight::PointLight(transform::Transform* transform) : Light(transform, LightManager::Point) { }
