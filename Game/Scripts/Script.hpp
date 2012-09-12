#ifndef	__SCRIPT_HPP__
#define	__SCRIPT_HPP__

#include "Script_Model.hpp"
#include "Actions/Action.h"
#include <queue>
#include "../../Constantes.h"
#include "../Game.h"
#include "../Unit.h"

inline double	TICK2SEC(uint32_t t)
{
	return (double)(t / MAX_TPS);
}
inline double	TICK2MSEC(uint32_t t)
{
	return (double)(t * 1000 / MAX_TPS);
}
inline uint32_t	SEC2TICK(double s)
{
	return (s * MAX_TPS);
}
inline uint32_t	MSEC2TICK(uint32_t ms)
{
	return (ms * MAX_TPS / 1000);
}

class	Script
{
	public:
		Script(Script_Model* m, Unit* caster) : _model(m),
							_currentAction(NULL),
							_lastCall(0),
							_tick(0),
							_caster(caster),
							_x(0),
							_y(0),
							_target(NULL)
		{
		}
		~Script()
		{
		}

		virtual void	Call(uint16_t x, uint16_t y, Unit* data)
		{
			int		i;
			uint32_t	tick;

			std::cout << "Call()\n";
			tick = Game::GetInstance()->currentTick;
			if (this->_lastCall + this->_model->GetCooldown() > tick)
			{
				// UPDATE SPRITE?
				std::cout << "Call()->CoolDown\n";
				return ;
			}
			while (!this->_queueActions.empty())
			{
				delete this->_queueActions.front();
				this->_queueActions.pop();
			}
			i = 1;
			this->_model->_actions[0]->Init();
			this->_currentAction = this->_model->_actions[0]->Clone();
			while (this->_model->_actions[i] != 0)
			{
				this->_model->_actions[i]->Init();
				this->_queueActions.push(this->_model->_actions[i]->Clone());
				++i;
			}
			this->_lastCall = tick;
			this->_tick = 0;
			this->_x = x;
			this->_y = y;
			this->_target = data;
			this->_caster->CallScript(this);
		}

		virtual int	Apply(void)
		{
			int	retAct;

			if (this->_tick < this->_model->GetCastTime())
			{
				// UPDATE SPRITE?
				++this->_tick;
				return 0;
			}
			retAct = this->_currentAction->Apply(this);
			while (retAct & AP_END)
			{
				if (retAct & AP_REDUNDANT)
					this->_queueActions.push(this->_currentAction->GetParent()->Clone());
				if (this->_queueActions.empty())
				{
					// DELETE FROM LIST
					return 1;
				}
				delete this->_currentAction;
				this->_currentAction = this->_queueActions.front();
				this->_queueActions.pop();
				retAct = this->_currentAction->Apply(this);
			}
			++this->_tick;
			return 0;
		}

		inline Script_Model*	GetModel(void) const
		{
			return this->_model;
		}
		inline uint32_t		GetTick(void) const
		{
			return this->_tick;
		}
		inline Unit*		GetCaster(void) const
		{
			return this->_caster;
		}
		inline uint32_t		GetX(void) const
		{
			return this->_x;
		}
		inline uint32_t		GetY(void) const
		{
			return this->_y;
		}
		inline Unit*		GetTarget(void) const
		{
			return this->_target;
		}

	protected:
		Script_Model*		_model;
		Action*			_currentAction;
		std::queue<Action*>	_queueActions;
		uint32_t		_lastCall;
		uint32_t		_tick;
		Unit*			_caster;
		uint16_t		_x;
		uint16_t		_y;
		Unit*			_target;
};

#endif
