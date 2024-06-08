#include "main/GameEngine/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"

using namespace component;
using namespace component;

IResetable::IResetable()
{
	GameManager::GetInstance()->AddResetable(this);
}

IResetable::~IResetable()
{
	GameManager::GetInstance()->DeleteResetable(this);
}
