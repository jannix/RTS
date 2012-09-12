#include "Join_Screen.h"
#include "../Env.h"
#include "../Constantes.h"
#include "../Game/Game.h"
#include "../Network/Sockets_manager.h"
#include "../Network/Socket.h"
#include "../Network/Protocol_Manager.h"
#include "Main_Screen.h"
#include <iostream>

void	__Join_Screen__joinGameCbClickDown(ID::Control*, ID::Event*, void* d)
{
	std::cout << "joinGame\n";
	Protocol_Manager*	pm;
	Sockets_manager*	sm;
	Socket*			skt;
	Join_Screen*		js;

	js = (Join_Screen*)d;
	sm = Sockets_manager::GetInstance();
	skt = sm->ConnectSocket(PORT, js->inputIP.GetText());
	if (skt == NULL)
		std::cerr << "Cant create socket" << std::endl;
	else
	{
		std::cout << "Connection established\n";
		sm->SetHostSocket(skt);
		pm = Protocol_Manager::GetInstance();
		pm->prot.AddCmdLobbyJoin();
	}
}

void	__Join_Screen__backCbClickDown(ID::Control*, ID::Event*, void*)
{
	std::cout << "back\n";
	Env*			e;

	e = Env::GetInstance();
	e->SwitchScreen(e->mainScreen);
}

Join_Screen::Join_Screen() : Abstract_Screen(UI_WIDTH, UI_HEIGHT),
			     joinTitle(DEFAULT_FONT_PATH, 40,
				       (char*)"Join Server",
				       (ID::Color*)&(ID::COLOR_GOLD)),
			     inputIP(500, 30, UI_FLAGS, UI_RMASK,
				     UI_GMASK, UI_BMASK, UI_AMASK,
				     DEFAULT_FONT_PATH, 28,
				     (ID::Color*)&ID::COLOR_ORANGE),
			     joinGame(130, 30, UI_FLAGS, UI_RMASK,
				       UI_GMASK, UI_BMASK, UI_AMASK,
				       DEFAULT_FONT_PATH, 20,
				       (char*)"Join Game",
				       (ID::Color*)&(ID::COLOR_GOLD)),
			     back(80, 30, UI_FLAGS, UI_RMASK,
				  UI_GMASK, UI_BMASK, UI_AMASK,
				  DEFAULT_FONT_PATH, 20, (char*)"Back",
				  (ID::Color*)&(ID::COLOR_GOLD))
{
	this->SetName("Join_Screen");

	this->joinTitle.SetPos(15, 15);
	this->AddChild(&(this->joinTitle));

	this->inputIP.SetPos(ID::REL_POS_CENTER);
	this->inputIP.SetBackground(0xFFFFFFFF);
	this->inputIP.SetMaxLength(15);
	this->inputIP.SetCbAction(__Join_Screen__joinGameCbClickDown, this);
	this->AddChild(&(this->inputIP));

	this->joinGame.SetPos(ID::REL_POS_RIGHT);
	this->joinGame.SetCbClickDown(__Join_Screen__joinGameCbClickDown, this);
	this->AddChild(&(this->joinGame));

	this->back.SetPos(ID::REL_POS_BOT);
	this->back.SetCbClickDown(__Join_Screen__backCbClickDown, NULL);
	this->AddChild(&(this->back));
}

Join_Screen::~Join_Screen()
{
}

void	Join_Screen::ApplyEvent(ID::Event* e)
{
//	Com_protocol		pm;
	Protocol_Manager	Pm;
	Protocol	p;
	int		fd;
//	char		buf[512];

	if (Sockets_manager::GetInstance()->GetSockets()->begin() != Sockets_manager::GetInstance()->GetSockets()->end())
	{
		fd = (*Sockets_manager::GetInstance()->GetSockets()->begin())->fd;
	p.AddCmdGameCreateUnit(1, 2, 2, 1);
	p.AddCmdGameCreateUnit(1, 3, 7, 2);
	p.AddCmdGameCreateUnit(2, 34, 16, 5);
	p.AddCmdGameDeleteUnit(4, 4);
	p.AddCmdGameDeleteUnit(4, 6);
	p.AddCmdGameOrderAttack(2, 3, 4, 5);
	p.AddCmdGameOrderMove(10, 11, 2, 3);
	p.AddCmdGameOrderMove(1, 2, 3, 4);
	p.AddCmdGameOrderAttack(4, 2, 8, 7);
	p.AddCmdGameSetGroup(43);
	p.AddCmdGameUseGroup(10, 1);
	p.AddCmdGameOrderAttack(7, 8, 9, 10);
//skill c'est pas fini
//	p.AddCmdGameBuild(24, 34);
	}
	if (e->type == ID::EVENT_KEY_DOWN)
	{
		if (e->keyDown.keysym.sym == ID::KEY_ESCAPE)
			__Join_Screen__backCbClickDown(0, 0, 0);
		else if (e->keyDown.keysym.sym == ID::KEY_a)
		{
			if (Sockets_manager::GetInstance()->GetSockets()->begin() != Sockets_manager::GetInstance()->GetSockets()->end())
			{
//				Pm.Write(fd, &p);
			}
		}
	}
}

void	Join_Screen::Refresh()
{
}

void	Join_Screen::Enter()
{
	Env*	e;
	std::cout << "Entering JoinScreen\n";

	e = Env::GetInstance();
	e->window->SetDrawableFocused(&(e->joinScreen->inputIP));
}

void	Join_Screen::Leave()
{
	Env*	e;
	std::cout << "Leaving JoinScreen\n";

	e = Env::GetInstance();
	e->window->SetDrawableFocused(NULL);
}

