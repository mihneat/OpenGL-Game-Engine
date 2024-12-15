#include "PropellerRotation.h"

using namespace component;

void PropellerRotation::Update(float deltaTime)
{
    transform->Rotate(normalize(rotationAxis) * speed * deltaTime);
}
