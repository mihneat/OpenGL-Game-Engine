#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

#include "lab_m1/Homework/3 - Steep/ComponentBase/Components/Logic/Managers/GameManager.h"

using namespace managers;
using namespace component;

IResetable::IResetable()
{
	GameManager::GetInstance()->AddResetable(this);
}

IResetable::~IResetable()
{
	GameManager::GetInstance()->DeleteResetable(this);
}
