#include "main/GameEngine/ComponentBase/Components/Logic/Managers/Interfaces/IResetable.h"

#include "main/GameEngine/ComponentBase/Components/Logic/Managers/GameManager.h"
#include "main/GameEngine/Managers/GameInstance.h"

using namespace component;

IResetable::IResetable()
{
	managers::GameInstance::Get()->GetComponent<GameManager>()->AddResetable(this);
}

IResetable::~IResetable()
{
	managers::GameInstance::Get()->GetComponent<GameManager>()->DeleteResetable(this);
}
