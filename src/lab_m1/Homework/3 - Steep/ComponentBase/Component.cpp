#include "lab_m1/Homework/3 - Steep/ComponentBase/Component.h"

using namespace std;
using namespace component;
using namespace transform;

Component::Component(Transform *transform)
{
	this->transform = transform;
	hasStartActivated = false;
	enabled = true;
}


Component::~Component()
{
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
