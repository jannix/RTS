#include "Unit.h"
#include "Game.h"
#include "Player.h"
#include "Sprite_Anime.h"
#include "Group.h"
#include "Map.h"
#include "Cam.h"
#include "AI_AStar.h"
#include "ChecksCollision.h"
#include "HUDEventsCallbacks.h"
#include "Move_Handlers/Ground_Move_Handler.h"
#include "Attack_Handlers/Ground_Attack_Handler.h"
#include "Scripts/Script.hpp"
#include "../ID_glibrary_Extension/Map_Drawable.h"
#include "../Env.h"
#include <iostream>

void	__Unit_CbClickDown(ID::Control* _u, ID::Event* e, void*)
{
	std::cout << "UCD\n";
	Env*	env;
	Unit*	u;

	u = (Unit*)_u;
	env = Env::GetInstance();
	Selector_CbClickDown(e, env->gameScreen);
	Order_CbClickDown(e, u);
}

void	__Unit_CbClickUp(ID::Control* _u, ID::Event* e, void*)
{
	std::cout << "UCU\n";
	Env*	env;
	Unit*	u;

	u = (Unit*)_u;
	env = Env::GetInstance();
	Selector_CbClickUp(e, env->gameScreen);
}

void	__Unit_CbMotion(ID::Control* _u, ID::Event* e, void*)
{
	Env*	env;
	Unit*	u;

	u = (Unit*)_u;
	env = Env::GetInstance();
	Selector_CbMotion(e, env->gameScreen);
	MapScrolling_CbMotion(e, env->gameScreen);
}

Unit::Unit(Unit_Model* Model, Player* p) : _player(p),
					   _model(Model),
					   _token(0),
					   _targetAttack(NULL),
					   _updateSaver(-1),
					   _lastRefreshCam(0),
					   _path(NULL)
{
	std::vector<Script_Model*>*		scripts;
	std::vector<Script_Model*>::iterator	it;
	std::vector<Script_Model*>::iterator	end;
	Script*					script;
	int					i;

	if (p != NULL)
		p->AddUnit(this);
	this->_HP = Model->GetHPMax();
	this->_PM = Model->GetPMMax();
	std::cout << this->_model->GetName() << " is out" << std::endl;
	i = 0;
	while (i < MAX_GROUPS)
	{
		this->_groups[i] = NULL;
		++i;
	}
	this->_pos.x = 0;
	this->_pos.y = 0;
	this->_targetMove.x = -1;
	this->_targetMove.y = -1;
	this->_targetAttackXY.x = -1;
	this->_targetAttackXY.y = -1;

	this->_animation.SetSpriteAnime(Model->GetSpriteAnime());
	if (this->_animation.GetSpriteAnime() == NULL)
		throw 1100;

	this->__AllocateMoveHandler(this->_model->GetMoveHandler());
	this->__AllocateAttackHandler(this->_model->GetAttackHandler());

	this->_stateMachine = new State_Machine(this);
	if (this->_stateMachine == NULL)
		throw 1130;
	this->_stateMachine->SetStates(2, &(this->idle), &(this->move));
	this->_stateMachine->SetInitialState(IDLE);

	scripts = this->_model->GetScripts();
	it = scripts->begin();
	end = scripts->end();
	while (it != end)
	{
		script = new Script(*it, this);
		if (script == NULL)
			throw 1140;
		this->_scripts.push_back(script);
		if (script->GetModel()->GetTriggeredAtStart() == TAS_TRUE)
			script->Call(0, 0, NULL);
		++it;
	}

	this->SetCbClickDown(__Unit_CbClickDown, this);
	this->SetCbClickUp(__Unit_CbClickUp, this);
	this->SetCbMotion(__Unit_CbMotion, this);
}

Unit::~Unit()
{
	this->__DeleteFromMap(this->_pos.x * CASE_SIDE_PIXELS, this->_pos.y * CASE_SIDE_PIXELS);
	this->__EraseFromPlayer();
	delete this->_moveHandler;
	delete this->_attackHandler;
	delete this->_stateMachine;
}

void	Unit::Update(void)
{
	Game*				gm;
	ID::vector<Script*>::iterator	it;
	ID::vector<Script*>::iterator	end;

	gm = Game::GetInstance();
	it = this->_runningScripts.begin();
	end = this->_runningScripts.end();
	while (it != end)
	{
		(*it)->Apply();
		++it;
	}
	this->_stateMachine->Update();
}

void	Unit::SetHP(uint32_t data)
{
	this->_HP = data;
}

void	Unit::SetPM(uint32_t data)
{
	this->_PM = data;
}

void	Unit::SetToken(uint32_t data)
{
	this->_token = data;
}

void	Unit::SetPosition(Position data)
{
	this->SetPosition(data.x, data.y);
}

void	Unit::SetPosition(double x, double y)
{
	Cam*		c;
	ID::Rect	camFOV;
	int		_x;
	int		_y;

	c = Cam::GetInstance();
	camFOV.x = c->GetX() - c->GetCameraWidthRayon();
	camFOV.y = c->GetY() - c->GetCameraHeightRayon();
	camFOV.w = camFOV.x + c->GetCameraWidth();
	camFOV.h = camFOV.y + c->GetCameraHeight();

	c->InvalidateCases(this->_pos.x, this->_pos.y, this->_model->GetSize());
	this->__DeleteFromMap(this->_pos.x * CASE_SIDE_PIXELS, this->_pos.y * CASE_SIDE_PIXELS);
	this->_pos.x = x;
	this->_pos.y = y;
	_x = x * CASE_SIDE_PIXELS;
	_y = y * CASE_SIDE_PIXELS;
/*	std::cout << "SetPos("
		<< _x << ";" << _y << ") C ("
		<< camFOV.x << ";" << camFOV.y << ";" << camFOV.w << ";" << camFOV.h << ")"
		<< "\n";*/
	if (camFOV.x <= _x && _x <= camFOV.w &&
	    camFOV.y <= _y && _y <= camFOV.h)
	{
		c->GetMapDrawable()->DelGround(this, this->GetModel()->GetGround());
		c->GetMapDrawable()->AddGround(this, this->GetModel()->GetGround());
		this->SetPos(_x - c->GetOffsetX(),
			     _y - c->GetOffsetY());
	}
	c->InvalidateCases(this->_pos.x, this->_pos.y, this->_model->GetSize());
	this->__AddToMap(_x, _y);
}

void	Unit::SetUpdateSaver(uint32_t ls)
{
	this->_updateSaver = ls;
}

void	Unit::SetPlayer(Player* data)
{
	if (this->_player == data)
		return ;
	this->__EraseFromPlayer();
	this->_player = data;
}

void	Unit::SetTargetMove(double x, double y)
{
	this->_targetMove.x = x;
	this->_targetMove.y = y;
}

void	Unit::SetTargetAttackUnit(Unit* u)
{
	this->_targetAttack = u;
}

void	Unit::SetTargetAttackPos(double x, double y)
{
	this->_targetAttackXY.x = x;
	this->_targetAttackXY.y = y;
}

uint32_t	Unit::GetHP(void)
{
	return (this->_HP);
}

uint32_t	Unit::GetPM(void)
{
	return (this->_PM);
}

uint32_t	Unit::GetToken(void)
{
	return (this->_token);
}

Position	Unit::GetPosition(void)
{
	return (this->_pos);
}

void		Unit::GetPositionXY(double* x, double* y)
{
	if (x != NULL)
		*x = this->_pos.x;
	if (y != NULL)
		*y = this->_pos.y;
}

uint32_t	Unit::GetUpdateSaver(void)
{
	return this->_updateSaver;
}

Unit_Model*	Unit::GetModel(void)
{
	return this->_model;
}

Player*	Unit::GetPlayer(void)
{
	return (this->_player);
}

Group**	Unit::GetGroups(void)
{
	return &(this->_groups[0]);
}

void	Unit::GetTargetMove(double* x, double* y)
{
	if (x != NULL)
		*x = this->_targetMove.x;
	if (y != NULL)
		*y = this->_targetMove.y;
}

Unit*	Unit::GetTargetAttackUnit(void)
{
	return this->_targetAttack;
}

void	Unit::GetTargetAttackPos(double* x, double* y)
{
	if (x != NULL)
		*x = this->_targetAttackXY.x;
	if (y != NULL)
		*y = this->_targetAttackXY.y;
}

void	Unit::CallScript(Script* s)
{
	this->_runningScripts.push_back(s);
}

void	Unit::RemoveScript(Script* s)
{
	ID::vector<Script*>::iterator	it;
	ID::vector<Script*>::iterator	end;

	it = this->_runningScripts.begin();
	end = this->_runningScripts.end();
	while (it != end)
	{
		if (*it == s)
		{
			if (this->_runningScripts.size() > 1)
			{
				*it = this->_runningScripts.back();
			}
			this->_runningScripts.pop_back();
			return ;
		}
		++it;
	}
}

Move_Handler*	Unit::GetMoveHandler(void)
{
	return this->_moveHandler;
}

Attack_Handler*	Unit::GetAttackHandler(void)
{
	return this->_attackHandler;
}

int	Unit::RefreshToSurface(ID::Surface* s, int x, int y)
{
	if (this->_animation.GetSpriteAnime() == NULL)
		return 0;
	if (this->_animation.
		GetSpriteAnime()->
		GetSprite()->
		BlitSurface(&(this->_animation.
			    GetSpriteAnime()->
			    GetRects()[this->_animation.Animate()]),
			    s,
			    x + this->_x - (this->GetWidth() >> 1),
			    y + this->_y - (this->GetHeight() >> 1)) == -1)
		return -1;
	this->SetInvalidate(false);
	return 0;
}

/*void	Unit::SetPos(int x, int y)
{
	this->_AddUpdateRect();
	this->_x = x;
	this->_y = y;
	this->_AddUpdateRect();
}*/

bool	Unit::_OnClickDown(ID::Event* event)
{
	if (this->GetOnScreen() == true &&
	    this->GetEnable() == true &&
	    event->clickDown.x >= this->_x + this->_offset_x - this->_model->GetSize() &&
	    event->clickDown.y >= this->_y + this->_offset_y - this->_model->GetSize() &&
	    event->clickDown.x < this->_x + this->_offset_x - this->_model->GetSize() + this->GetWidth() &&
	    event->clickDown.y < this->_y + this->_offset_y - this->_model->GetSize() + this->GetHeight())
		return true;
	return false;
}

bool	Unit::_OnClickUp(ID::Event* event)
{
	if (this->GetOnScreen() == true &&
	    this->GetEnable() == true &&
	    event->clickUp.x >= this->_x + this->_offset_x - this->_model->GetSize() &&
	    event->clickUp.y >= this->_y + this->_offset_y - this->_model->GetSize() &&
	    event->clickUp.x < this->_x + this->_offset_x - this->_model->GetSize() + this->GetWidth() &&
	    event->clickUp.y < this->_y + this->_offset_y - this->_model->GetSize() + this->GetHeight())
		return true;
	return false;
}

void	Unit::__AddToMap(int16_t x, int16_t y)
{
	std::cout << "POS(" << x << ", " << y << ")";
//	Cam*				cam;
	Map*				map;
	std::list<Unit*>::iterator	it;
	std::list<Unit*>::iterator	end;
	std::vector<Unit*>		selection;
	uint16_t	w;
	uint16_t	h;
	int16_t		xC;
	int16_t		yC;
	uint16_t	wC;
	uint16_t	hC;
	int		i;
//	int		j;

	ID::Circle	c;
	ID::Rect	r;
//	Light_Drawable*	cases;

	c.center.x = x;
	c.center.y = y;
	c.radius = this->_model->GetSize();

	map = Map::GetInstance();
//	cam = Cam::GetInstance();

	r.w = CASE_SIDE_PIXELS;
	r.h = CASE_SIDE_PIXELS;

//	x -= cam->GetOffsetX();
//	y -= cam->GetOffsetY();
	w = x + this->_model->GetSize();
	h = y + this->_model->GetSize();
	x -= this->_model->GetSize();
	y -= this->_model->GetSize();
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;

	xC = x >> 5;
	yC = y >> 5;
	wC = w >> 5;
	hC = h >> 5;
	i = yC * map->w + xC;
	std::cout << "ADDTOMAP("
		<< xC << "->"
		<< wC << "|"
		<< yC << "->"
		<< hC
		<< ")	I=" << i
		<< "\n";
//	cases = cam->GetCases();
//	j = cam->GetCameraWidth() >> 5;
	while (yC <= hC)
	{
		xC = x >> 5;
		i = yC * map->w + xC;
		while (xC <= wC)
		{
			r.x = (xC << 5) + (r.w >> 1);
			r.y = (yC << 5) + (r.h >> 1);
			if (CheckCollision_RectCircle(&r, &c))
			{
			//	printf("		%d,%d\n",r.x,r.y);
				map->cases[i].AddUnit(this);
//				cases[yC * j + xC].SetInvalidate(true);
			}
			++i;
			++xC;
		}
		++yC;
	}
}

void	Unit::__DeleteFromMap(int16_t x, int16_t y)
{
	std::cout << "POS(" << x << ", " << y << ")";
//	Cam*				cam;
	Map*				map;
	std::list<Unit*>::iterator	it;
	std::list<Unit*>::iterator	end;
	std::vector<Unit*>		selection;
	uint16_t	w;
	uint16_t	h;
	int16_t		xC;
	int16_t		yC;
	uint16_t	wC;
	uint16_t	hC;
	int		i;
//	int		j;

	ID::Circle	c;
	ID::Rect	r;
//	Light_Drawable*	cases;

	c.center.x = x;
	c.center.y = y;
	c.radius = this->_model->GetSize();

	map = Map::GetInstance();
//	cam = Cam::GetInstance();

	r.w = CASE_SIDE_PIXELS;
	r.h = CASE_SIDE_PIXELS;

//	x -= cam->GetOffsetX();
//	y -= cam->GetOffsetY();
	w = x + this->_model->GetSize();
	h = y + this->_model->GetSize();
	x -= this->_model->GetSize();
	y -= this->_model->GetSize();
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;

	xC = x >> 5;
	yC = y >> 5;
	wC = w >> 5;
	hC = h >> 5;
	i = yC * map->w + xC;
	std::cout << "DELETEFROMMAP("
		<< xC << "->"
		<< wC << "|"
		<< yC << "->"
		<< hC
		<< ")	I=" << i
		<< "\n";
//	cases = cam->GetCases();
//	j = map->w >> 5;
	while (yC <= hC)
	{
		xC = x >> 5;
		i = yC * map->w + xC;
		while (xC <= wC)
		{
			r.x = (xC << 5) + (r.w >> 1);
			r.y = (yC << 5) + (r.h >> 1);
			if (CheckCollision_RectCircle(&r, &c))
			{
				map->cases[i].DelUnit(this);
//				cases[yC * j + xC].SetInvalidate(true);
			}
			++i;
			++xC;
		}
		++yC;
	}
}

void	Unit::__EraseFromPlayer(void)
{
	if (this->_player != NULL)
		this->_player->DelUnit(this->_token);
}

void	Unit::__AllocateMoveHandler(int ID)
{
	if (ID == MHDLER_IMMOBILE)
	{
		this->_moveHandler = new Ground_Move_Handler(this);
		if (this->_moveHandler == NULL)
			throw 1112;
	}
	else if (ID == MHDLER_GROUND)
	{
		this->_moveHandler = new Ground_Move_Handler(this);
		if (this->_moveHandler == NULL)
			throw 1114;
	}
	else if (ID == MHDLER_AIR)
	{
		this->_moveHandler = new Ground_Move_Handler(this);
		if (this->_moveHandler == NULL)
			throw 1116;
	}

	else
		throw 1119;
}

void	Unit::__AllocateAttackHandler(int ID)
{
	if (ID == AHDLER_NOATTACK)
	{
		this->_attackHandler = new Ground_Attack_Handler(this);
		if (this->_attackHandler == NULL)
			throw 1120;
	}
	else if (ID == AHDLER_GROUND)
	{
		this->_attackHandler = new Ground_Attack_Handler(this);
		if (this->_attackHandler == NULL)
			throw 1120;
	}
	else if (ID == AHDLER_AIR)
	{
		this->_attackHandler = new Ground_Attack_Handler(this);
		if (this->_attackHandler == NULL)
			throw 1120;
	}
	else if (ID == AHDLER_BOTH)
	{
		this->_attackHandler = new Ground_Attack_Handler(this);
		if (this->_attackHandler == NULL)
			throw 1120;
	}
	else
		throw 1125;
}

