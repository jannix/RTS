#include "Cam.h"
#include "Map.h"
#include "Unit.h"
#include "HUDEventsCallbacks.h"
#include "../Env.h"
#include "../Screens/Game_Screen.h"
#include "../ID_glibrary_Extension/Light_Drawable.h"
#include "../ID_glibrary_Extension/Map_Drawable.h"
#include "ChecksCollision.h"
#include <list>
#include <sys/time.h>
#include "../Chrono.hpp"

#include "../Network/Protocol_Manager.h"
#include "../Database/Database.h"
#include "Unit_Model.h"
#include "Game.h"

void	__LD__CbClickDown(ID::Control* _ld, ID::Event* e, void* _gs)
{
	std::cout << "CD\n";
	Light_Drawable*	ld;
	Game_Screen*	gs;

	ld = (Light_Drawable*)_ld;
	gs = (Game_Screen*)_gs;
	Selector_CbClickDown(e, gs);
	Order_CbClickDown(e, ld);
}

void	__LD__CbClickUp(ID::Control* _ld, ID::Event* e, void* _gs)
{
	std::cout << "CU\n";
	Light_Drawable*	ld;
	Game_Screen*	gs;

	ld = (Light_Drawable*)_ld;
	gs = (Game_Screen*)_gs;
	Selector_CbClickUp(e, gs);

	std::cout << "but=" << e->clickUp.button << std::endl;
	if (e->clickUp.button == 4)
	{
		Protocol_Manager*	pm;

		pm = Protocol_Manager::GetInstance();
		pm->prot.AddCmdGameCreateUnit(1,
					      e->clickUp.x >> 5,
					      e->clickUp.y >> 5,
					      0);
/*		db = Database::GetInstance();

		um = db->getUnitModel(1);
		u = new Unit(um);
		u->SetName("Unit");
		u->SetPosition(e->clickUp.x >> 5, e->clickUp.y >> 5);
*/	}
	else if (e->clickUp.button == 2)
	{
		Protocol_Manager*	pm;

		pm = Protocol_Manager::GetInstance();
		pm->prot.AddCmdGameCreateUnit(2,
					      e->clickUp.x >> 5,
					      e->clickUp.y >> 5,
					      0);
/*		db = Database::GetInstance();
		um = db->getUnitModel(2);
		u = new Unit(um, Game::GetInstance()->players[Game::GetInstance()->myID]);
		u->SetName("Unit");
		u->SetPosition(e->clickUp.x >> 5, e->clickUp.y >> 5);
		std::cout << "ADD("
			<< (e->clickUp.x >> 5)
			<< ","
			<< (e->clickUp.y >> 5)
			<< ")"
			<< std::endl;
*/	}
}

void	__LD__CbMotion(ID::Control* _ld, ID::Event* e, void* _gs)
{
//	std::cout << "M\n";
	Light_Drawable*	ld;
	Game_Screen*	gs;

	ld = (Light_Drawable*)_ld;
	gs = (Game_Screen*)_gs;
	Selector_CbMotion(e, gs);
	MapScrolling_CbMotion(e, gs);
}

Cam::Cam() : b_left(false), b_right(false), b_top(false), b_down(false),
	     __x(0), __y(0),
	     __nX(0), __nY(0),
	     mapDrawable(NULL),
	     cases(NULL),
	     __toRefresh(false)
{
	if (pthread_mutex_init(&(this->__camMutex), NULL) != 0)
		throw 459;
	this->__chrono.SaveTime();
}

Cam::~Cam()
{
	if (pthread_mutex_destroy(&(this->__camMutex)) != 0)
	{
		std::cerr << "Error Cam: pthread_mutex_destroy failed\n";
	}
	delete[] this->cases;
	delete this->mapDrawable;
	std::cout << "Cam::Destructor()\n";
}

int	Cam::Init(ID::Window* win)
{
	Game_Screen*	gs;

	this->cameraWidth = win->GetWidth();
	this->cameraWidthRayon = this->cameraWidth >> 1;
	this->cameraHeight = win->GetHeight() - ((win->GetHeight() * 0.2 / 32 * 32));
	this->cameraHeightRayon = this->cameraHeight >> 1;
	this->nb_case_screen = (this->cameraWidth >> 5) * (this->cameraHeight >> 5);

	this->mapDrawable = new Map_Drawable(9, this->cameraWidth, this->cameraHeight);
	if (this->mapDrawable == NULL)
		return -1;
	gs = Env::GetInstance()->gameScreen;
	gs->AddChild(this->mapDrawable);
	gs->MoveChildBack(&(gs->selector));
	return 0;
}

int	Cam::Init(ID::Drawable* win)
{
	Game_Screen*	gs;

	this->cameraWidth = win->GetWidth();
	this->cameraWidthRayon = this->cameraWidth >> 1;
	this->cameraHeight = win->GetHeight() - ((win->GetHeight() * 0.2 / 32 * 32));
	this->cameraHeightRayon = this->cameraHeight >> 1;
	this->nb_case_screen = (this->cameraWidth >> 5) * (this->cameraHeight >> 5);

	this->mapDrawable = new Map_Drawable(9, this->cameraWidth, this->cameraHeight);
	if (this->mapDrawable == NULL)
		return -1;
	gs = Env::GetInstance()->gameScreen;
	gs->AddChild(this->mapDrawable);
	gs->MoveChildBack(&(gs->selector));
	return 0;
}

void	Cam::SetCam(int nX, int nY)
{
	this->__nX = nX;
	this->__nY = nY;
	this->__toRefresh = true;
}

void	Cam::MoveCam(int x_c, int y_c)
{
	this->SetCam(this->__x + x_c, this->__y + y_c);
}

void	Cam::SetMapData(Map* m)
{
	this->mapWidth = m->w << 5;
	this->mapHeight = m->h << 5;
}

int	Cam::InitMapDrawable(void)
{
	Game_Screen*	gs;
	int	i;

	i = 0;
	while (i < 9)
	{
		this->mapDrawable->ClearGround(i);
		++i;
	}

	delete[] this->cases;
	this->cases = new Light_Drawable[this->nb_case_screen];
	if (this->cases == NULL)
		return -1;

	gs = Env::GetInstance()->gameScreen;
	i = 0;
	while (i < this->nb_case_screen)
	{
		this->cases[i].SetPos((i % (this->cameraWidth >> 5)) << 5,
				      (i / (this->cameraWidth >> 5)) << 5);
		this->cases[i].SetCbClickDown(__LD__CbClickDown, gs);
		this->cases[i].SetCbClickUp(__LD__CbClickUp, gs);
		this->cases[i].SetCbMotion(__LD__CbMotion, gs);
		this->mapDrawable->AddGround(&this->cases[i], BACKGROUND);
		++i;
	}
	this->__x = 0;
	this->__y = 0;
	return 0;
}

void	Cam::RefreshScrolling()
{
	if (this->__chrono.CompareLast() > 40000)
		this->__chrono.SaveTime();
	else
		return ;

	if (this->b_top)
	{
	printf("reffreshscrol\n");
		this->__nY -= 32;
		this->__toRefresh = true;
	}
	else if (this->b_down)
	{
		this->__nY += 32;
		this->__toRefresh = true;
	}
	if (this->b_left)
	{
		this->__nX -= 32;
		this->__toRefresh = true;
	}
	else if (this->b_right)
	{
		printf("b_right cam\n");
		this->__nX += 32;
		this->__toRefresh = true;
	}
}

void	Cam::Refresh()
{
	if (this->__toRefresh == false)
		return ;

	if (this->__nX < this->cameraWidthRayon)
		this->__nX = this->cameraWidthRayon;
	else if (this->__nX > this->mapWidth - this->cameraWidthRayon)
		this->__nX = this->mapWidth - this->cameraWidthRayon;
	if (this->__nY < this->cameraHeightRayon)
		this->__nY = this->cameraHeightRayon;
	else if (this->__nY > this->mapHeight - this->cameraHeightRayon)
		this->__nY = this->mapHeight - this->cameraHeightRayon;
	if (this->__x == this->__nX && this->__y == this->__nY)
	{
		this->__toRefresh = false;
		return ;
	}
	Chrono	c;

	int				i;
	int				j;
	int				x;
	int				y;
	int				xMax;
	int				yMax;
	int				offsetCamX;
	int				offsetCamY;
	uint32_t			updateSaver;
	Position			posUnit;
	std::list<Unit*>::iterator	itU;
	std::list<Unit*>::iterator	endU;
	Map*				map;
	Env*				env;

	c.SaveTime();
	map = Map::GetInstance();
	env = Env::GetInstance();
	env->window->AddUpdate(0);
	this->__x = this->__nX;
	this->__y = this->__nY;
	offsetCamX = this->GetOffsetX();
	offsetCamY = this->GetOffsetY();
	xMax = this->cameraWidth >> 5;
	yMax = this->cameraHeight >> 5;
	i = ((this->__y >> 5) - (this->cameraHeightRayon >> 5)) *
	    (this->mapWidth >> 5) + ((this->__x >> 5) - (this->cameraWidthRayon >> 5));
	j = 0;
	y = 0;
	this->mapDrawable->ClearGround(MIDDLEGROUND);
	this->mapDrawable->ClearGround(HIGHGROUND);
	updateSaver = c.SaveTime(); // Get unique number
	while (y < yMax)
	{
		x = 0;
		while (x < xMax)
		{
			this->cases[j].SetPicture(&(map->cases[i].animation));
			if (this->cases[j].GetPicture()->GetSpriteAnime() == NULL)
				std::cout << "E\n";
			itU = map->cases[i].units.begin();
			endU = map->cases[i].units.end();
			//#TODO Faire different refresh pour les differentes niveaux de la map
			while (itU != endU)
			{
				if ((*itU)->GetUpdateSaver() != updateSaver)
				{
					(*itU)->SetUpdateSaver(updateSaver);
					posUnit = (*itU)->GetPosition();
					(*itU)->SetPos((int)(posUnit.x * CASE_SIDE_PIXELS) - offsetCamX,
						       (int)(posUnit.y * CASE_SIDE_PIXELS) - offsetCamY);
					this->mapDrawable->AddGround(*itU,
								     (*itU)->GetModel()->GetGround());
				}
				++itU;
			}
			++j;
			++i;
			++x;
		}
		i = i - xMax + map->w;
		++y;
	}
	this->__toRefresh = false;
}

uint32_t	Cam::GetOffsetX(void)
{
	return ((this->__x / CASE_SIDE_PIXELS) -
		(this->cameraWidthRayon / CASE_SIDE_PIXELS)) * CASE_SIDE_PIXELS;
	return ((this->__x - this->cameraWidthRayon) / CASE_SIDE_PIXELS) * CASE_SIDE_PIXELS;
	return this->__x - this->cameraWidthRayon;
}

uint32_t	Cam::GetOffsetY(void)
{
	return ((this->__y / CASE_SIDE_PIXELS) -
		(this->cameraHeightRayon / CASE_SIDE_PIXELS)) * CASE_SIDE_PIXELS;
	return ((this->__y - this->cameraHeightRayon) / CASE_SIDE_PIXELS) * CASE_SIDE_PIXELS;
	return this->__y - this->cameraHeightRayon;
}

void	Cam::InvalidateCases(float _x, float _y, int size)
{
	std::list<Unit*>::iterator	it;
	std::list<Unit*>::iterator	end;
	int16_t				xMin;
	int16_t				xMax;
	uint16_t			yMin;
	uint16_t			yMax;
	int16_t				deltaX;
	int16_t				x;
	int				i;
//	int				j;
//	int				pitch;

	ID::Circle	c;
	ID::Rect	r;

	c.center.x = _x * CASE_SIDE_PIXELS;
	c.center.y = _y * CASE_SIDE_PIXELS;
	c.center.x -= this->GetOffsetX();
	c.center.y -= this->GetOffsetY();
	if (c.center.x < 0 || c.center.y < 0)
		return ;
	c.radius = size;
	r.w = size + size;
	r.h = size + size;

	xMin = (int)(c.center.x - size) >> 5;
	xMax = (int)(c.center.x + size) >> 5;
	yMin = (int)(c.center.y - size) >> 5;
	yMax = (int)(c.center.y + size) >> 5;
	if (xMax >= this->cameraWidth >> 5 ||
	    yMax >= this->cameraHeight >> 5)
		return ;

//	j = Map::GetInstance()->w >> 5;
	deltaX = xMax - xMin;
	i = yMin * (this->cameraWidth >> 5) + xMin;
	while (yMin <= yMax)
	{
		std::cout << "ICase-Y(" << yMin << ")";
		x = xMin;
		while (x <= xMax)
		{
			r.x = (x << 5) + (r.w >> 1);
			r.y = (yMin << 5) + (r.h >> 1);
			std::cout << "	X" << x
				<< "	"
				<< r.x
				<< ";"
				<< r.y;
			if (CheckCollision_RectCircle(&r, &c) &&
			    this->cases[i].GetInvalidate() == false)
			{
				std::cout << "Case[" << i << "],";
				this->cases[i].SetInvalidate(true);
			}
			++x;
			++i;
		}
		i = i + (this->cameraWidth >> 5) - deltaX - 1;
		yMin++;
		std::cout << "\n";
	}
}

