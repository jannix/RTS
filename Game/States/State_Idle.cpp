#include "State_Idle.h"
#include "State_Machine.h"
#include "../Unit.h"
#include "../Move_Handlers/Move_Handler.h"

#include <iostream>

State_Idle::State_Idle()
{
}

State_Idle::~State_Idle()
{
}

void	State_Idle::Update(void)
{
	double	x;
	double	y;

	this->motherMachine->GetUnit()->GetTargetMove(&x, &y);
	if (x != -1 && y != -1)
	{
		this->motherMachine->SetNextState(MOVE);
	}
	else
	{
		// AJOUTE ATTACK ICI ANTOINE
		// Tu regardes dans la Map::GetInstance() qui est dans ta vision.
		// Ensuite tu changes de state.
	}
}

void	State_Idle::OnEnter(void)
{
	std::cout << "OnEnter IDLE\n";
}

void	State_Idle::OnLeave(void)
{
	std::cout << "OnLeave IDLE\n";
}

