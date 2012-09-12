#include "HUDEventsCallbacks.h"
#include "ChecksCollision.h"
#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "Cam.h"
#include "Unit.h"
#include "Move_Handlers/Move_Handler.h"
#include "AI_AStar.h"
#include "../Env.h"
#include "../Screens/Game_Screen.h"
#include "../Network/Protocol_Manager.h"
#include <vector>
#include <sys/time.h>

void	Selector_CbClickDown(ID::Event* e, Game_Screen* gs)
{
	if (e->clickDown.button != ID::BUTTON_LEFT)
		return ;
	std::cout << "ClickDown\n";
	gs->selector.SetOnScreen(true);
	gs->selector.SetDimension(1, 1);
	gs->xSelector = e->clickDown.x;
	gs->ySelector = e->clickDown.y;
	gs->nXSelector = e->clickDown.x;
	gs->nYSelector = e->clickDown.y;
	gs->refreshSelector = true;
	gs->movingSelector = true;
}

void	Selector_CbClickUp(ID::Event* e, Game_Screen* gs)
{
	if (e->clickUp.button != ID::BUTTON_LEFT ||
	    gs->movingSelector == false)
		return ;
	std::cout << "ClickUp\n";
	gs->selector.SetOnScreen(false);
	gs->movingSelector = false;
	GetSelection(&(gs->selector));
}

void	Selector_CbMotion(ID::Event* e, Game_Screen* gs)
{
	if (e->motion.state != ID::BUTTON_LEFT ||
	    gs->movingSelector == false)
		return ;
	if (gs->selector.GetOnScreen() == true)
	{
		gs->nXSelector = e->motion.x;
		gs->nYSelector = e->motion.y;
		gs->refreshSelector = true;
		return ;
	}
}

void	Order_CbClickDown(ID::Event* e, Light_Drawable*)
{
	if (e->clickDown.button != ID::BUTTON_RIGHT)
		return ;
	//#TODO Ajouter check de script, si le joueur est  en train de lancer un script ou pas, sinon faire l'ordre de mouvement
	std::cout << "Order_CbClickDown(LD)\n";
	Protocol_Manager*		pm;
	Game*				gm;
	std::list<Unit*>::iterator	it;
	std::list<Unit*>::iterator	end;
	std::list<Unit*>*		selection;
	Cam*				c;
	double				offsetCamX;
	double				offsetCamY;

	gm = Game::GetInstance();
	pm = Protocol_Manager::GetInstance();
	c = Cam::GetInstance();
	offsetCamX = c->GetOffsetX() / CASE_SIDE_PIXELS;
	offsetCamY = c->GetOffsetY() / CASE_SIDE_PIXELS;
	selection = gm->players[gm->myID]->GetSelection();
	it = selection->begin();
	end = selection->end();
	while (it != end)
	{
		std::cout << "AddCmdGameOrderMove()" << std::endl;
		pm->prot.AddCmdGameOrderMove(offsetCamX + e->clickDown.x / CASE_SIDE_PIXELS,
					     offsetCamY + e->clickDown.y / CASE_SIDE_PIXELS,
					     (*it)->GetToken(),
					     gm->myID);
//		(*it)->SetTargetMove(offsetCamX + e->clickDown.x / CASE_SIDE_PIXELS,
//				     offsetCamY + e->clickDown.y / CASE_SIDE_PIXELS);
		++it;
	}
}

void	Order_CbClickDown(ID::Event* e, Unit*)
{
	if (e->clickDown.button != ID::BUTTON_RIGHT)
		return ;
	//#TODO Ajouter check de script, si le joueur est pas en train de lancer un script ou pas, sinon bouger ou attaquer si ennemi.
	std::cout << "Order_CbClickDown(UNIT)\n";
}

void	MapScrolling_CbMotion(ID::Event* e, Game_Screen* _gs)
{
	Cam*	c;
	Game_Screen*	gs;

	gs = (Game_Screen*)_gs;
	c = Cam::GetInstance();
	if (e->motion.x <= 10)
		c->b_left = true;
	else if (e->motion.x > 10)
		c->b_left = false;
	if (e->motion.x >= c->GetCameraWidth() - 10)
		c->b_right = true;
	else if (e->motion.x < c->GetCameraWidth() - 10)
		c->b_right = false;
	if (e->motion.y <= 15)
		c->b_top = true;
	else if (e->motion.y > 15)
		c->b_top = false;
	if (e->motion.y >= Env::GetInstance()->window->GetHeight() - 20)
		c->b_down = true;
	else if (e->motion.y < Env::GetInstance()->window->GetHeight() - 20)
		c->b_down = false;
}

void	GetSelection(ID::GRect* sel)
{
	Protocol_Manager*		pm;
	Game*				gm;
	Env*				env;
	Cam*				c;
	Map*				map;
	std::list<Unit*>::iterator	it;
	std::list<Unit*>::iterator	end;
	int16_t		x;
	int16_t		y;
	uint16_t	w;
	uint16_t	h;
	int16_t		xC;
	int16_t		yC;
	uint16_t	wC;
	uint16_t	hC;
	int		i;
	int		j;
	uint32_t	updateSaver;
	Position	pos;
	ID::Rect	selector;
	ID::Rect	unit;
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return ;

	pm = Protocol_Manager::GetInstance();
	c = Cam::GetInstance();
	map = Map::GetInstance();
	gm = Game::GetInstance();
	env = Env::GetInstance();

	sel->GetPos(&x, &y);
	x += c->GetOffsetX();
	y += c->GetOffsetY();
	w = sel->GetWidth() + 1;
	h = sel->GetHeight() + 1;
	std::cout << "GetSelection("
		<< x << ","
		<< y << ")"
		<< w << "|"
		<< h
		<< "\n";
	selector.x = x;
	selector.y = y;
	selector.w = w;
	selector.h = h;
	xC = x >> 5;
	yC = y >> 5;
	wC = (x + w) >> 5;
	hC = (y + h) >> 5;
	i = yC * map->w + xC;
	std::cout << "("
		<< xC << ","
		<< yC << ")"
		<< wC << "|"
		<< hC
		<< "	I=" << i
		<< "\n";
	gm->players[gm->myID]->ClearSelection();
	updateSaver = tv.tv_usec;
	j = c->GetCameraWidth() >> 5;
	while (yC <= hC)
	{
		xC = x >> 5;
		i = yC * map->w + xC;
		while (xC <= wC)
		{
			it = map->cases[i].units.begin();
			end = map->cases[i].units.end();
			while (it != end)
			{
		//#TODO ajouter une mutexounette.
	//	gm->LockMutex();
				pos = (*it)->GetPosition();
				unit.w = (*it)->GetModel()->GetSize() << 1;
				unit.h = (*it)->GetModel()->GetSize() << 1;
				unit.x = (pos.x * 32) - (*it)->GetModel()->GetSize();
				unit.y = (pos.y * 32) - (*it)->GetModel()->GetSize();
				std::cout << "Unit("
					<< unit.x
					<< ","
					<< unit.y
					<< ","
					<< unit.w
					<< ")"
					<< std::endl;
				if ((*it)->GetUpdateSaver() != updateSaver &&
				    (*it)->GetPlayer() == gm->GetMe() &&
				    CheckCollision_RectRect(&selector, &unit))
				{
					gm->players[gm->myID]->AddUnit2Selection(*it);
					std::cout << "Add\n";
				}
	//	gm->UnlockMutex();
				(*it)->SetUpdateSaver(updateSaver);
				++it;
			}
			++i;
			++xC;
		}
		++yC;
	}
	if (gm->GetMe()->GetSelection()->size() > 0)
	{
		std::cout << "AddcmdGameSel(" << gm->players[gm->myID] << ")\n";
		pm->prot.AddCmdGameSelection(gm->players[gm->myID]);
		env->gameScreen->RefreshSelectedList();
	}
}
