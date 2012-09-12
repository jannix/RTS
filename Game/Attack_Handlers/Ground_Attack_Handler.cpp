#include "Ground_Attack_Handler.h"
#include "../Game.h"
#include "../Unit.h"
#include "../Player.h"
#include "../Map.h"
#include "../Math_tools.h"
#include "../../Constantes.h"

Ground_Attack_Handler::Ground_Attack_Handler(Unit* u) : Attack_Handler(u)
{
}

Ground_Attack_Handler::~Ground_Attack_Handler()
{
}

bool	Ground_Attack_Handler::CanAttack(Unit* u)
{
	double	xD;
	double	yD;
	double	xA;
	double	yA;
	double	dist;

	if ((u->GetModel()->GetType() == TYPE_AIR && (this->_unit->GetModel()->GetTargetType() != TARGET_ALL ||
	    this->_unit->GetModel()->GetTargetType() != TARGET_AIR)) ||
	    (u->GetModel()->GetType() == TYPE_GROUND && (this->_unit->GetModel()->GetTargetType() != TARGET_ALL ||
	    this->_unit->GetModel()->GetTargetType() != TARGET_GROUND)))
		return false;
	u->GetPositionXY(&xD, &yD);
	this->_unit->GetPositionXY(&xA, &yA);
	dist = IDdistance(xD, yD, xA, yA);
	dist -= u->GetModel()->GetSize();
	dist -= this->_unit->GetModel()->GetSize();
	dist -= this->_unit->GetModel()->GetArange();
	return (dist <= 0);
}

int	Ground_Attack_Handler::Attack(Unit* u)
{
	if (!this->CanAttack(u))
		return 0;
	uint32_t	hp;

	hp = u->GetHP() - (this->_unit->GetModel()->GetAtk() - u->GetModel()->GetDef());
	if (hp <= 0)
	{
		delete u; // Ca fait tout automatiquement normalement.
	}
	u->SetHP(hp);
	return 1;
}

