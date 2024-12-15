#include "Marker.h"

#include "core/engine.h"


void component::Marker::Start()
{
    pointer = transform->GetChild(1);
    pointerInitialPosition = pointer->GetLocalPosition();
}

void component::Marker::Update(float deltaTime)
{
    const float elapsedTime = Engine::GetElapsedTime();

    pointer->SetLocalPosition(pointerInitialPosition + glm::vec3(0, glm::sin(elapsedTime * speed), 0));
    pointer->Rotate(glm::vec3(0, 1, 0) * rotationSpeed * deltaTime);
    
}
