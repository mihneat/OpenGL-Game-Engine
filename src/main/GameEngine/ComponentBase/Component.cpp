#include "main/GameEngine/ComponentBase/Component.h"

using namespace std;
using namespace component;
using namespace transform;

Component::Component(Transform *transform)
{
	this->transform = transform;
}


Component::~Component()
{
}

bool Component::GetHasAwakeActivated()
{
	return hasAwakeActivated;
}

void Component::SetHasAwakeActivated()
{
	hasAwakeActivated = true;
}

bool Component::GetHasStartActivated()
{
	return hasStartActivated;
}

void Component::SetHasStartActivated()
{
	hasStartActivated = true;
}

bool Component::IsActive()
{
	return enabled;
}

void Component::SetActive(bool active)
{
	enabled = active;
}
