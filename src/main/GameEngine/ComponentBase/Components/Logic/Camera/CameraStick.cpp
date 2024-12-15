#include "CameraStick.h"

using namespace component;

void CameraStick::Start()
{
    helicopter = transform->GetTransformByTag("Helicopter");
    
    relativeOffset = transform->GetWorldPosition() - helicopter->GetWorldPosition();
}

void CameraStick::Update(const float deltaTime)
{
    transform->SetLocalPosition(helicopter->GetWorldPosition() + relativeOffset);
}
