#include "main/GameEngine/ComponentBase/Components/Rendering/Lights/DirectionalLight.h"
#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/Managers/LightManager.h"

#include <iostream>

using namespace std;
using namespace loaders;
using namespace component;
using namespace transform;

DirectionalLight::DirectionalLight(Transform* transform) : Light(transform)
{
    Light::ChangeType(LightManager::Directional);
}
