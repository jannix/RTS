#ifndef	__STATE_ATTACK_H__
#define	__STATE_ATTACK_H__

#include "State.h"
//#include "../Unit.h"

class	Unit;
struct	Position;

class	State_Attack : public State
{
	public:
		State_Attack();
		~State_Attack();

		virtual void	Update(void);
		virtual void	OnEnter(void);
		virtual void	OnLeave(void);

	public:
		Unit*		targetUnit;
		Position*	targetPos;
//Position:
};

#endif
