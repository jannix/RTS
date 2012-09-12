#include "State_Machine.h"
#include "State.h"

State_Machine::State_Machine(Unit* u) :__unit(u), __currentState(NULL), __nextState(-1)
{
}

State_Machine::~State_Machine()
{
}

void	State_Machine::Update(void)
{
	if (this->__nextState != -1)
	{
		this->SwitchState(this->__nextState);
		this->__nextState = -1;
	}
	this->__currentState->Update();
}

void	State_Machine::SetNextState(int stateID)
{
	this->__nextState = stateID;
}

void	State_Machine::SwitchState(int stateID)
{
	this->__currentState->OnLeave();
	this->__currentState = this->__finiteStatesList[stateID];
	this->__currentState->OnEnter();
}

int	State_Machine::SetStates(int n, ...)
{
	va_list	args;
	State*	state;
	int	i;

	va_start(args, n);
	this->__finiteStatesList.resize(n);
	i = 0;
	while (i < n)
	{
		state = va_arg(args, State*);
		state->motherMachine = this;
		this->__finiteStatesList[i] = state;
		++i;
	}
	va_end(args);
	return 0;
}

void	State_Machine::SetInitialState(int i)
{
	this->__currentState = this->__finiteStatesList[i];
}
