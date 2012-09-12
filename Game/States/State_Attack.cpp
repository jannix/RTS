#include "State_Attack.h"
#include "State_Machine.h"
#include "../Game.h"
#include "../Player.h"
#include "../Attack_Handlers/Attack_Handler.h"
#include "../../Constantes.h"
#include "../Unit.h"

#include <iostream>

State_Attack::State_Attack()
{
}

State_Attack::~State_Attack()
{
}

void	State_Attack::Update(void)
{
	Unit*	u;

	u = this->motherMachine->GetUnit();
	if (this->targetUnit != NULL)
	{
		if (u->GetAttackHandler()->Attack(this->targetUnit) == 0)
			this->motherMachine->SetNextState(MOVE); // TK Forbidden
		// GO STATE MOVE_focusAttack
	}
	else
	{
		// GO STATE MOVE_focusAttack
	}
}

void	State_Attack::OnEnter(void)
{
	printf("on enter attack\n");
	Unit*		u;
	Game*		gm;

	gm = Game::GetInstance();
	u = this->motherMachine->GetUnit();
	this->targetUnit = u->GetTargetAttackUnit();
	if (this->targetUnit != NULL)
	{
		if (u->GetPlayer()->GetID() == gm->myID ||
		    u->GetPlayer()->GetTeam() == gm->players[gm->myID]->GetTeam())
			this->motherMachine->SetNextState(IDLE); // TK Forbidden
		return ;
	}
	else
	{
		u->GetTargetAttackPos(&(this->targetPos->x), &(this->targetPos->y));
		if (this->targetPos->x == -1 &&
		    this->targetPos->y == -1)
		{
			this->motherMachine->SetNextState(IDLE); // NO TARGET, CANCEL STATE
		}
	}
}

void	State_Attack::OnLeave(void)
{
}

