#include "Game_Screen.h"
#include "Main_Screen.h"
#include "../Env.h"
#include "../Tools.h"
#include "../Constantes.h"
#include "../Menu/Main_Menu.h"
#include "../Game/Map.h"
#include "../Game/Game.h"
#include "../Game/Cam.h"
#include "../Game/HUDEventsCallbacks.h"
#include "../Database/Database.h"
#include "../ID_glibrary_Extension/Map_Drawable.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <SDL/SDL_rotozoom.h>

#include "../Game/Unit.h"
#include "../Game/Player.h"
#include "../Game/Building.h"
#include "../Game/Unit_Model.h"
#include "../Game/Production_DB.h"
#include "../Database/Database.h"

#define HUD_BUTTON_SIZE 50
#define HUD_SELECTED_SIZE 50

void	__Game_Screen__HUDCbMotion(ID::Control*, ID::Event* e, void* _gs)
{
	Game_Screen*	gs;

	gs = (Game_Screen*)_gs;
	Selector_CbMotion(e, gs);
	MapScrolling_CbMotion(e, gs);
}

void	__Game_Screen__HUDCbClickDown(ID::Control*, ID::Event* e, void* _gs)
{
	Game_Screen*	gs;

	gs = (Game_Screen*)_gs;
	Selector_CbClickDown(e, gs);
}

void	__Game_Screen__HUDCbClickUp(ID::Control*, ID::Event* e, void* _gs)
{
	Game_Screen*	gs;

	gs = (Game_Screen*)_gs;
	Selector_CbClickUp(e, gs);
}

inline void	SetCam(ID::Drawable* d, int x, int y)
{
	Cam*	c;
	int	offsetX;
	int	offsetY;
	int16_t	posX;
	int16_t	posY;
	int	caseX;
	int	caseY;

	c = Cam::GetInstance();
	d->GetOffset(&offsetX, &offsetY);
	d->GetPos(&posX, &posY);
	caseX = c->GetMapWidth() * (x - offsetX - posX) / d->GetWidth();
	caseY = c->GetMapHeight() * (y - offsetY - posY) / d->GetHeight();
	c->SetCam(caseX, caseY);
}

void	__Game_Screen__minimapCbClickDown(ID::Control* mm, ID::Event* e, void* _gs)
{
	if (e->clickDown.button & SDL_BUTTON_LEFT)
	{
		Game_Screen*	gs;

		gs = (Game_Screen*)_gs;
		gs->movingCam = true;
		SetCam((ID::Drawable*)mm, e->clickDown.x, e->clickDown.y);
	}
}

void	__Game_Screen__minimapCbClickUp(ID::Control*, ID::Event* e, void* _gs)
{
	Game_Screen*	gs;

	gs = (Game_Screen*)_gs;
	if (e->clickDown.button & SDL_BUTTON_LEFT)
		gs->movingCam = false;
	Selector_CbClickUp(e, gs);
}

void	__Game_Screen__minimapCbMotion(ID::Control* mm, ID::Event* e, void* _gs)
{
	Game_Screen*	gs;

	gs = (Game_Screen*)_gs;
	if (e->motion.state & SDL_BUTTON_LEFT)
	{
		if (gs->movingCam == true)
			SetCam((ID::Drawable*)mm, e->motion.x, e->motion.y);
	}
	Selector_CbMotion(e, gs);
	MapScrolling_CbMotion(e, gs);
}

void	__Game_Screen_orderFocus(ID::Button_Label* newFocus)
{
	Env*	env;
	env = Env::GetInstance();

	if (env->gameScreen->orderFocus != NULL)
	{
		env->gameScreen->orderFocus->SetBorder((uint16_t)0);
		env->gameScreen->orderFocus->SetBorderColor(0xFF0000);
	}
	env->gameScreen->orderFocus = newFocus;
	env->gameScreen->orderFocus->SetBorder(1);
	env->gameScreen->orderFocus->SetBorderColor(0xFF0000);
}

void	__Game_Screen__moveCbClickDown(ID::Control*, ID::Event*, void*)
{
	std::cout << "move\n";
	Env*	env;
	env = Env::GetInstance();

	__Game_Screen_orderFocus(&env->gameScreen->b1);
}

void	__Game_Screen__attackCbClickDown(ID::Control*, ID::Event*, void*)
{
	std::cout << "attack\n";
	Env*	env;
	env = Env::GetInstance();

	__Game_Screen_orderFocus(&env->gameScreen->b2);
}

void	__Game_Screen__patrolCbClickDown(ID::Control*, ID::Event*, void*)
{
	std::cout << "patrol\n";
	Env*	env;
	env = Env::GetInstance();

	__Game_Screen_orderFocus(&env->gameScreen->b3);
}

void	__Game_Screen__holdCbClickDown(ID::Control*, ID::Event*, void*)
{
	std::cout << "hold\n";
	Env*	env;
	env = Env::GetInstance();

	__Game_Screen_orderFocus(&env->gameScreen->b4);
}

void	__Game_Screen__assistCbClickDown(ID::Control*, ID::Event*, void*)
{
	std::cout << "assist\n";
	Env*	env;
	env = Env::GetInstance();

	__Game_Screen_orderFocus(&env->gameScreen->b5);
}

void	__Game_Screen__stopCbClickDown(ID::Control*, ID::Event*, void*)
{
	std::cout << "stop\n";
	Env*	env;
	env = Env::GetInstance();

	__Game_Screen_orderFocus(&env->gameScreen->b6);
}



ID::Color	selectorColor = ID::Color(0xFF, 0xB0, 0x0F, 0x7F);

Game_Screen::Game_Screen() : Abstract_Screen(UI_WIDTH, UI_HEIGHT),
				HUD(UI_WIDTH, (UI_HEIGHT / 5) + 20 , UI_FLAGS,
				    UI_RMASK, UI_GMASK, UI_BMASK, UI_AMASK),
				b1(HUD_BUTTON_SIZE, HUD_BUTTON_SIZE,
					UI_FLAGS, UI_RMASK, UI_GMASK,
					UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
					10, (char*)" ", (ID::Color*)&(ID::COLOR_GOLD)),
				b2(HUD_BUTTON_SIZE, HUD_BUTTON_SIZE,
					UI_FLAGS, UI_RMASK, UI_GMASK,
					UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
					10, (char*)" ", (ID::Color*)&(ID::COLOR_GOLD)),
				b3(HUD_BUTTON_SIZE, HUD_BUTTON_SIZE,
					UI_FLAGS, UI_RMASK, UI_GMASK,
					UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
					10, (char*)" ", (ID::Color*)&(ID::COLOR_GOLD)),
				b4(HUD_BUTTON_SIZE, HUD_BUTTON_SIZE,
					UI_FLAGS, UI_RMASK, UI_GMASK,
					UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
					10, (char*)" ", (ID::Color*)&(ID::COLOR_GOLD)),
				b5(HUD_BUTTON_SIZE, HUD_BUTTON_SIZE,
					UI_FLAGS, UI_RMASK, UI_GMASK,
					UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
					10, (char*)" ", (ID::Color*)&(ID::COLOR_GOLD)),
				b6(HUD_BUTTON_SIZE, HUD_BUTTON_SIZE,
					UI_FLAGS, UI_RMASK, UI_GMASK,
					UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
					10, (char*)" ", (ID::Color*)&(ID::COLOR_GOLD)),
				b7(HUD_BUTTON_SIZE, HUD_BUTTON_SIZE,
					UI_FLAGS, UI_RMASK, UI_GMASK,
					UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
					10, (char*)" ", (ID::Color*)&(ID::COLOR_GOLD)),
				b8(HUD_BUTTON_SIZE, HUD_BUTTON_SIZE,
					UI_FLAGS, UI_RMASK, UI_GMASK,
					UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
					10, (char*)" ", (ID::Color*)&(ID::COLOR_GOLD)),
				b9(HUD_BUTTON_SIZE, HUD_BUTTON_SIZE,
					UI_FLAGS, UI_RMASK, UI_GMASK,
					UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
					10, (char*)" ", (ID::Color*)&(ID::COLOR_GOLD)),
				_minimap((UI_WIDTH / 2), (UI_HEIGHT / 2) -
					(((UI_HEIGHT / 5) / 32) / 2), UI_FLAGS,
					 UI_RMASK, UI_GMASK, UI_BMASK, UI_AMASK),
				minimap((UI_WIDTH / 5), (UI_HEIGHT / 5) -
					(((UI_HEIGHT / 5) / 32) / 2), UI_FLAGS,
					 UI_RMASK, UI_GMASK, UI_BMASK, UI_AMASK),
				selector(0, 0, true, &selectorColor),
				infos(0)
{
	Env*		env;
	Database*	db;
	int		i;
	ID::Label*	lbl;

	env = Env::GetInstance();
	db = Database::GetInstance();

	this->SetAbstractParent(true);
	this->SetName("Game_Screen");

	this->HUD.SetPos(0, env->height - (env->height / 5) - 20);
	this->HUD.SetBackground((ID::Color*)&(ID::COLOR_GREY));
	this->HUD.SetCbMotion(__Game_Screen__HUDCbMotion, this);
	this->AddChild(&(this->HUD));


	this->b1.SetPos((env->width - ((HUD_BUTTON_SIZE + 5) * 3)),
			   (env->height - (HUD_BUTTON_SIZE + 5) * 3 - 20));
	this->b1.SetBackground(999999);
	this->AddChild(&(this->b1));

	this->b2.SetPos((env->width - ((HUD_BUTTON_SIZE + 5) * 2)),
			   (env->height - (HUD_BUTTON_SIZE + 5) * 3 - 20));
	this->b2.SetBackground(999999);
	this->AddChild(&(this->b2));

	this->b3.SetPos((env->width - ((HUD_BUTTON_SIZE + 5))),
			   (env->height - (HUD_BUTTON_SIZE + 5) * 3 - 20));
	this->b3.SetBackground(999999);
	this->AddChild(&(this->b3));

	this->b4.SetPos((env->width - ((HUD_BUTTON_SIZE + 5) * 3)),
			   (env->height - (HUD_BUTTON_SIZE + 5) * 2 - 20));
	this->b4.SetBackground(999999);
	this->AddChild(&(this->b4));

	this->b5.SetPos((env->width - ((HUD_BUTTON_SIZE + 5) * 2)),
			   (env->height - (HUD_BUTTON_SIZE + 5) * 2 - 20));
	this->b5.SetBackground(999999);
	this->AddChild(&(this->b5));

	this->b6.SetPos((env->width - ((HUD_BUTTON_SIZE + 5) )),
			   (env->height - (HUD_BUTTON_SIZE + 5) * 2 - 20));
	this->b6.SetBackground(999999);
	this->AddChild(&(this->b6));

	this->b7.SetPos((env->width - ((HUD_BUTTON_SIZE + 5) * 3 )),
			   (env->height - (HUD_BUTTON_SIZE + 5) - 20));
	this->b7.SetBackground(999999);
	this->AddChild(&(this->b7));

	this->b8.SetPos((env->width - ((HUD_BUTTON_SIZE + 5) * 2)),
			   (env->height - (HUD_BUTTON_SIZE + 5) - 20));
	this->b8.SetBackground(999999);
	this->AddChild(&(this->b8));

	this->b9.SetPos((env->width - ((HUD_BUTTON_SIZE + 5) )),
			   (env->height - (HUD_BUTTON_SIZE + 5) - 20));
	this->b9.SetBackground(999999);
	this->AddChild(&(this->b9));

	this->minimap.SetName("Minimap");
	this->minimap.SetPos(5, env->height - (env->height / 5) - 10);
	this->minimap.SetBackground(&(this->_minimap));
//	this->minimap.SetBackground((ID::Color*)&(ID::COLOR_WHITE));
	this->minimap.SetCbMotion(__Game_Screen__minimapCbMotion, this);
	this->minimap.SetCbClickDown(__Game_Screen__minimapCbClickDown, this);
	this->minimap.SetCbClickUp(__Game_Screen__minimapCbClickUp, this);
	this->AddChild(&(this->minimap));

	this->selector.SetOnScreen(false);
	this->AddChild(&(this->selector));

	this->orderFocus = 0;

	i = 1;
	while (db->getResourceModel(i) != NULL)
	{
		std::cout << "	--------		-----ADDLBEL\n";
		lbl = new ID::Label(DEFAULT_FONT_PATH, 20, (char*)"0");
		if (lbl == NULL)
			throw 1200;
		lbl->SetPos(UI_WIDTH / 5 + 15, 20 * i);
		this->HUD.AddChild(lbl);
		this->resources.push_back(lbl);
		++i;
	}
}


Game_Screen::~Game_Screen()
{
	std::vector<ID::Label*>::iterator	it;
	std::vector<ID::Label*>::iterator	end;

	it = this->resources.begin();
	end = this->resources.end();
	while (it != end)
	{
		delete *it;
		++it;
	}
}

void	Game_Screen::RefreshMinimap()
{
	Map*		map;
	int		x;
	int		y;
	int		i;
	int		case_w;
	int		case_h;

	map = Map::GetInstance();
	y = 0;
	case_w = (UI_WIDTH / 4) / map->w;
	case_h = (UI_HEIGHT / 5) - 10 / map->h;
	i = 0;
	while (y < map->h)
	{
		x = 0;
		while (x < map->w)
		{
			this->_minimap.SetPixel(x, y, &(map->cases[i].animation.GetSpriteAnime()->color_case));
			++x;
			++i;
		}
		++y;
	}
//	this->_minimap = zoomSurface(&(this->_minimap), 2, 2, 0);
}

void	Game_Screen::ApplyEvent(ID::Event* e)
{
	if (e->type == ID::EVENT_KEY_DOWN)
	{
		if (e->keyDown.keysym.sym == ID::KEY_ESCAPE)
		{
			Env*	env;
			env = Env::GetInstance();

			env->SwitchScreen(env->mainScreen);
			Game::GetInstance()->CleanAll();
		}
	}
}

inline void	RefreshSelector(Game_Screen* gs)
{
	if (gs->refreshSelector == false)
		return ;
	int16_t	newX;
	int16_t	newY;
	int16_t	newW;
	int16_t	newH;

	if (gs->nXSelector >= gs->xSelector)
	{
		newX = gs->xSelector;
		newW = gs->nXSelector - gs->xSelector;
	}
	else
	{
		newX = gs->nXSelector;
		newW = gs->xSelector - gs->nXSelector;
	}
	if (gs->nYSelector >= gs->ySelector)
	{
		newY = gs->ySelector;
		newH = gs->nYSelector - gs->ySelector;
	}
	else
	{
		newY = gs->nYSelector;
		newH = gs->ySelector - gs->nYSelector;
	}
	gs->selector.SetPos(newX, newY);
	gs->selector.SetDimension(newW, newH);
	gs->refreshSelector = false;
}

void	Game_Screen::UnitHUD()
{
	this->b1.GetLabel()->SetText((char*)"MOVE");
	this->b1.SetCbClickDown(__Game_Screen__moveCbClickDown, NULL);

	this->b2.GetLabel()->SetText((char*)"ATK");
	this->b2.SetCbClickDown(__Game_Screen__attackCbClickDown, NULL);

	this->b3.GetLabel()->SetText((char*)"PTRL");
	this->b3.SetCbClickDown(__Game_Screen__patrolCbClickDown, NULL);

	this->b4.GetLabel()->SetText((char*)"HOLD");
	this->b4.SetCbClickDown(__Game_Screen__holdCbClickDown, NULL);

	this->b5.GetLabel()->SetText((char*)"AST");
	this->b5.SetCbClickDown(__Game_Screen__assistCbClickDown, NULL);

	this->b6.GetLabel()->SetText((char*)"STOP");
	this->b6.SetCbClickDown(__Game_Screen__stopCbClickDown, NULL);

	this->b7.GetLabel()->SetText((char*)" ");
	this->b7.SetCbClickDown(0, NULL);

	this->b8.GetLabel()->SetText((char*)" ");
	this->b8.SetCbClickDown(0, NULL);

	this->b9.GetLabel()->SetText((char*)" ");
	this->b9.SetCbClickDown(0, NULL);
}

void	Game_Screen::VoidHUD()
{
	this->b1.GetLabel()->SetText((char*)" ");
	this->b1.SetCbClickDown(0, NULL);

	this->b2.GetLabel()->SetText((char*)" ");
	this->b2.SetCbClickDown(0, NULL);

	this->b3.GetLabel()->SetText((char*)" ");
	this->b3.SetCbClickDown(0, NULL);

	this->b4.GetLabel()->SetText((char*)" ");
	this->b4.SetCbClickDown(0, NULL);

	this->b5.GetLabel()->SetText((char*)" ");
	this->b5.SetCbClickDown(0, NULL);

	this->b6.GetLabel()->SetText((char*)" ");
	this->b6.SetCbClickDown(0, NULL);

	this->b7.GetLabel()->SetText((char*)" ");
	this->b7.SetCbClickDown(0, NULL);

	this->b8.GetLabel()->SetText((char*)" ");
	this->b8.SetCbClickDown(0, NULL);

	this->b9.GetLabel()->SetText((char*)" ");
	this->b9.SetCbClickDown(0, NULL);
}

void	OrderToBuild(ID::Control* mm, ID::Event* e, void* _data)
{
	(void)mm;
	(void)e;
	Production_DB*			data;
	Game*				game;
	std::list<Unit*>::iterator	it;

	data = (Production_DB*)_data;
	game = Game::GetInstance();

	it = game->GetMe()->GetSelection()->begin();
	((Building*)(*it))->AddToBuildList(data);
}

void	Game_Screen::ProdHUD(Building* u)
{
	Database*				db;
	std::vector<Production_DB*>		prodList;
	std::vector<Production_DB*>::iterator	it;
	int					i;

	db = Database::GetInstance();
	prodList = db->getProductionList(u->GetModel()->GetID());
	it = prodList.begin();
	i = 0;

	this->VoidHUD();

	if (it != prodList.end())
	{
		std::cout << (*it) << std::endl;
		this->b1.GetLabel()->SetText(db->getUnitModel((*it)->prodID)->GetName());
		this->b1.SetCbClickDown(OrderToBuild, (void*)(*it));
		++it;	
	}
	if (it != prodList.end())
	{
		this->b2.GetLabel()->SetText(db->getUnitModel((*it)->prodID)->GetName());
		this->b2.SetCbClickDown(OrderToBuild, (void*)(*it));
		++it;	
	}
	if (it != prodList.end())
	{
		this->b3.GetLabel()->SetText(db->getUnitModel((*it)->prodID)->GetName());
		this->b3.SetCbClickDown(0, NULL);
		++it;	
	}
	if (it != prodList.end())
	{
		this->b4.GetLabel()->SetText(db->getUnitModel((*it)->prodID)->GetName());
		this->b4.SetCbClickDown(0, NULL);
		++it;	
	}
	if (it != prodList.end())
	{
		this->b5.GetLabel()->SetText(db->getUnitModel((*it)->prodID)->GetName());
		this->b5.SetCbClickDown(0, NULL);
		++it;	
	}
	if (it != prodList.end())
	{
		this->b6.GetLabel()->SetText(db->getUnitModel((*it)->prodID)->GetName());
		this->b6.SetCbClickDown(0, NULL);
		++it;	
	}
	if (it != prodList.end())
	{
		this->b7.GetLabel()->SetText(db->getUnitModel((*it)->prodID)->GetName());
		this->b7.SetCbClickDown(0, NULL);
		++it;	
	}
	if (it != prodList.end())
	{
		this->b8.GetLabel()->SetText(db->getUnitModel((*it)->prodID)->GetName());
		this->b8.SetCbClickDown(0, NULL);
		++it;	
	}
	if (it != prodList.end())
	{
		this->b9.GetLabel()->SetText(db->getUnitModel((*it)->prodID)->GetName());
		this->b9.SetCbClickDown(0, NULL);
		++it;
	}
}

void	Game_Screen::Refresh()
{
	Cam*	c;

	c = Cam::GetInstance();
	c->Refresh();
	c->RefreshScrolling();
	RefreshSelector(this);
}


void	Game_Screen::Enter()
{
	std::cout << "Entering GameScreen\n";
	Cam*	c;
	Env*	e;

	c = Cam::GetInstance();
	e = Env::GetInstance();
//	e->window->AddChild(c->GetMapDrawable());
}

void	Game_Screen::Leave()
{
	std::cout << "Leaving GameScreen\n";
	Cam*	c;
	Env*	e;

	c = Cam::GetInstance();
	e = Env::GetInstance();
//	e->window->GetMainControl()->DelChild(c->GetMapDrawable());
}

void	Game_Screen::RefreshSelectedList()
{
	Env*					env;
	Game*					game;
	std::list<Unit*>::iterator		selected_it;
	std::list<HUD_Selected*>::iterator	selectedList_it;
	int					i = 0;

	if (this->infos == 0) 
		this->infos = new HUD_DisplayInfo();

	env = Env::GetInstance();
	game = Game::GetInstance();
	selected_it = game->GetMe()->GetSelection()->begin();
	selectedList_it = this->SelectedList.begin();

	while (selectedList_it != this->SelectedList.end())
	{
		delete ((*selectedList_it));
		++selectedList_it;
	}

	this->SelectedList.clear();

	while (selected_it != game->GetMe()->GetSelection()->end())
	{
		env->gameScreen->SelectedList.push_back(new HUD_Selected(i, (*selected_it)));
		++selected_it;
		++i;
	}
	if (i == 0)
		return;
	if ((*game->GetMe()->GetSelection()->begin())->GetModel()->GetType() != 3)
		this->UnitHUD();
	else if ((*game->GetMe()->GetSelection()->begin())->GetModel()->GetType() == 3)
		this->ProdHUD((Building*)(*(game->GetMe()->GetSelection()->begin())));
	else if (game->GetMe()->GetSelection()->begin() == game->GetMe()->GetSelection()->end())
		this->VoidHUD();

	if (i == 1)
	{
		this->infos->Update((*game->GetMe()->GetSelection()->begin()));
		this->infos->Show();
	}
	else
		this->infos->Hide();

}

HUD_Selected::HUD_Selected(int pos, Unit* unit) : button(HUD_SELECTED_SIZE, HUD_SELECTED_SIZE,
							UI_FLAGS, UI_RMASK, UI_GMASK,
							UI_BMASK, UI_AMASK, DEFAULT_FONT_PATH,
							10, unit->GetModel()->GetName(), (ID::Color*)&(ID::COLOR_GOLD)),
						  HPred(HUD_SELECTED_SIZE, 5, UI_FLAGS,
					 		UI_RMASK, UI_GMASK, UI_BMASK, UI_AMASK),
						  HPgreen((unit->GetHP() / unit->GetModel()->GetHPMax()) * HUD_SELECTED_SIZE, 5, UI_FLAGS,
					 		UI_RMASK, UI_GMASK, UI_BMASK, UI_AMASK)
{
	Env*	env;
	
	env = Env::GetInstance();

	this->LinkedUnit = unit;

	this->SetPos(350 + ((HUD_SELECTED_SIZE + 5) * pos), (env->height - (HUD_SELECTED_SIZE + 5) * 2));

//	this->move.SetCbClickDown(__Game_Screen__moveCbClickDown, NULL);
	this->button.SetBackground(999999);
	env->gameScreen->AddChild(&(this->button));

	this->HPred.SetBackground((ID::Color*)&(ID::COLOR_RED));
	env->gameScreen->AddChild(&(this->HPred));

	this->HPgreen.SetBackground((ID::Color*)&(ID::COLOR_GREEN));
	env->gameScreen->AddChild(&(this->HPgreen));
}

HUD_Selected::~HUD_Selected()
{
	Env*	env;

	env = Env::GetInstance();

	env->gameScreen->DelChild(&(this->button));	
	env->gameScreen->DelChild(&(this->HPred));	
	env->gameScreen->DelChild(&(this->HPgreen));	
}

void	HUD_Selected::SetPos(int x, int y)
{
	this->button.SetPos(x, y);
	this->HPred.SetPos(x, y + HUD_SELECTED_SIZE);
	this->HPgreen.SetPos(x, y + HUD_SELECTED_SIZE);	
}


HUD_DisplayInfo::HUD_DisplayInfo() : name(DEFAULT_FONT_PATH, 20, (char*)" "),
				     HP(DEFAULT_FONT_PATH, 20, (char*)" "),
				     ATK(DEFAULT_FONT_PATH, 20, (char*)" "),
				     DEF(DEFAULT_FONT_PATH, 20, (char*)" "),
				     lastUnit(0)
{
	Env*	env;
	env = Env::GetInstance();

	this->name.SetPos((env->width / 2),
			   (env->height - 40 * 4));
	this->name.SetOnScreen(false);
	env->gameScreen->AddChild(&(this->name));

	this->HP.SetPos((env->width / 2),
			   (env->height - 40 * 3));
	this->HP.SetOnScreen(false);
	env->gameScreen->AddChild(&(this->HP));

	this->ATK.SetPos((env->width / 2),
			   (env->height - 40 * 2));
	this->ATK.SetOnScreen(false);
	env->gameScreen->AddChild(&(this->ATK));

	this->DEF.SetPos((env->width / 2),
			   (env->height - 40));
	this->DEF.SetOnScreen(false);
	env->gameScreen->AddChild(&(this->DEF));
}

HUD_DisplayInfo::~HUD_DisplayInfo()
{
	Env*	env;
	env = Env::GetInstance();
}

void	HUD_DisplayInfo::Update(Unit* unit)
{
	char	hp[9];

	if (unit != this->lastUnit)
	{
		this->name.SetText(unit->GetModel()->GetName());
		this->ATK.SetText(id_itoa(unit->GetModel()->GetAtk()));
		this->DEF.SetText(id_itoa(unit->GetModel()->GetDef()));
	}
	strcpy(hp, id_itoa(unit->GetHP()));
	strcat(hp, " / ");
	strcat(hp, id_itoa(unit->GetModel()->GetHPMax()));
	this->HP.SetText(hp);
}


void	HUD_DisplayInfo::Show()
{
	this->name.SetOnScreen(true);
	this->HP.SetOnScreen(true);
	this->ATK.SetOnScreen(true);
	this->DEF.SetOnScreen(true);
}

void	HUD_DisplayInfo::Hide()
{
	this->name.SetOnScreen(false);
	this->HP.SetOnScreen(false);
	this->ATK.SetOnScreen(false);
	this->DEF.SetOnScreen(false);
}
