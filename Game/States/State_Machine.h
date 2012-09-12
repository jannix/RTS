#ifndef	__STATE_MACHINE_H__
#define	__STATE_MACHINE_H__

#include <queue>
#include <stdarg.h>
#include "../../vector.h"

class	State;
class	Unit;

class	State_Machine
{
	public:
			State_Machine(Unit*);
			~State_Machine();
		void	Update(void);
		void	SetNextState(int);
		void	SwitchState(int);
		int	SetStates(int n, ...);
		void	SetInitialState(int);

		inline Unit*	GetUnit(void)
		{ return this->__unit; }
	private:
		Unit*			__unit;
		State*			__currentState;
		int			__nextState;
		ID::vector<State*>	__finiteStatesList;
};

#endif
