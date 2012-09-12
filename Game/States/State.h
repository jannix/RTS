#ifndef __STATE_H__
#define __STATE_H__

#include <vector>

class	State_Machine;

class	State
{
	friend class	State_Machine;
 public:
	State();
	~State();

	virtual void	Update(void) = 0;
	virtual void	OnEnter(void);
	virtual void	OnLeave(void);

 protected:
	State_Machine*	motherMachine;
/*	void	SetID(int);
	void	SetName(char*);
	void	SetAction(int);
	void	SetCausedBy(int);

	int	GetID();
	char*	GetName();
	int	GetAction();
	int	GetCausedBy();

	void	AddEffect(int);
	void	DelEffect(int);
 private:
	int			ID;
	char*			Name;
	int			Action;
	int			CausedBy;
	std::vector<int>	WillEffect;
*/

};

#endif
