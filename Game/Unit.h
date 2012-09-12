#ifndef __UNIT_H__
#define __UNIT_H__

enum	MOVE_HANDLER
{
	MHDLER_IMMOBILE = 0,
	MHDLER_GROUND,
	MHDLER_AIR
};

enum	ATTACK_HANDLER
{
	AHDLER_NOATTACK = 0,
	AHDLER_GROUND,
	AHDLER_AIR,
	AHDLER_BOTH
};

// Temp locatin - Enum for state machine
enum	STATE
{
	IDLE = 0,
	MOVE,
	ATTACK
};

struct	Position
{
	double	x;
	double	y;
};

#include "Unit_Model.h"
#include "States/State_Machine.h"
#include "States/State_Idle.h"
#include "States/State_Move.h"
#include "States/State_Attack.h"
#include "../Constantes.h"
#include "../ID_glibrary_Extension/Light_Drawable.h"
#include "../vector.h"

class	Player;
class	Group;
class	Sprite_Anime;
class	Node;
class	Move_Handler;
class	Attack_Handler;
class	Script;

class	Unit : public Light_Drawable
{
 public:
	Unit(Unit_Model*, Player* = NULL);
	~Unit();
	virtual void	Update(void);

	void		SetHP(uint32_t data);
	void		SetPM(uint32_t data);
	void		SetToken(uint32_t data);
	void		SetPosition(Position data);
	void		SetPosition(double x, double y);
	void		SetUpdateSaver(uint32_t);
	void		SetPlayer(Player* data);
	void		SetTargetMove(double x, double y);
	void		SetTargetAttackUnit(Unit*);
	void		SetTargetAttackPos(double x, double y);

	uint32_t	GetHP(void);
	uint32_t	GetPM(void);
	uint32_t	GetToken(void);
	Position	GetPosition(void);
	void		GetPositionXY(double* x, double* y);
	uint32_t	GetUpdateSaver(void);
	Unit_Model*	GetModel(void);
	Player*		GetPlayer(void);
	Group**		GetGroups(void);
	void		GetTargetMove(double* x, double* y);
	Unit*		GetTargetAttackUnit();
	void		GetTargetAttackPos(double* x, double* y);

	void		CallScript(Script*);
	void		RemoveScript(Script*);

	Move_Handler*	GetMoveHandler(void);
	Attack_Handler*	GetAttackHandler(void);

	virtual	int	RefreshToSurface(ID::Surface*, int x, int y);
//	void		SetPos(int x, int y); AJOUTER

//	virtual int 	ClickDown(ID::Event* event);
//	virtual int 	ClickUp(ID::Event* event);

	virtual bool	_OnClickDown(ID::Event* event);
	virtual bool	_OnClickUp(ID::Event* event);

 protected:
	Group*		_groups[MAX_GROUPS];
	Player*		_player;
	Unit_Model*	_model;
	uint32_t	_HP;
	uint32_t	_PM;
	uint32_t	_token;
	Position	_pos;
	Position	_targetMove;
	Position	_targetAttackXY;
	Unit*		_targetAttack;
	uint32_t	_updateSaver;
	uint32_t	_lastRefreshCam;
	Node*		_path;
	Move_Handler*	_moveHandler;
	Attack_Handler*	_attackHandler;
	State_Machine*	_stateMachine;
	ID::vector<Script*>	_scripts;
	ID::vector<Script*>	_runningScripts;

 private:
	void	__AddToMap(int16_t x, int16_t y);
	void	__DeleteFromMap(int16_t x, int16_t y);
	void	__EraseFromPlayer(void);
	void	__AllocateMoveHandler(int ID);
	void	__AllocateAttackHandler(int ID);

	// Temporary States Location
	State_Idle	idle;
	State_Move	move;
	State_Attack	attack;
};

#endif
