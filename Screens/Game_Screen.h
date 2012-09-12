#ifndef	__GAME_SCREEN_H__
#define	__GAME_SCREEN_H__

#include "../ID_glibrary.h"
#include "Abstract_Screen.hpp"

class	Unit;
class	Building;

class	HUD_DisplayInfo
{
	public:
		ID::Label	name;
		ID::Label	HP;
		ID::Label	ATK;
		ID::Label	DEF;
		Unit*		lastUnit;
	public:
		HUD_DisplayInfo();
		~HUD_DisplayInfo();
		void	Update(Unit* unit);
		void	Show();
		void	Hide();
};

class	HUD_Selected
{
	public:
		ID::Button_Label	button;
		ID::Drawable		HPred;
		ID::Drawable		HPgreen;
		Unit*			LinkedUnit;
	public:
		HUD_Selected(int pos, Unit* unit);
		~HUD_Selected();
		void	SetPos(int x, int y);
};

class	Game_Screen : public Abstract_Screen
{
	public:
				Game_Screen();
				~Game_Screen();
		virtual	void	ApplyEvent(ID::Event*);
		virtual	void	Refresh();
		virtual	void	Enter();
		virtual	void	Leave();
			void	RefreshMinimap();
			void	RefreshSelectedList();
			void	Selected_Delete(int unit_token);
			void	ProdHUD(Building*);
			void	UnitHUD();
			void	VoidHUD();

	public:
		ID::Drawable		HUD;
		std::vector<ID::Label*>	resources;
		ID::Button_Label		b1;
		ID::Button_Label		b2;
		ID::Button_Label		b3;
		ID::Button_Label		b4;
		ID::Button_Label		b5;
		ID::Button_Label		b6;
		ID::Button_Label		b7;
		ID::Button_Label		b8;
		ID::Button_Label		b9;
		ID::Surface		_minimap;
		ID::Drawable		minimap;
		bool			movingCam;
		bool			movingSelector;
		ID::Button_Label*	orderFocus;
		ID::GRect		selector;
		uint16_t		xSelector;
		uint16_t		ySelector;
		uint16_t		nXSelector;
		uint16_t		nYSelector;
		bool			refreshSelector;
		std::list<HUD_Selected*>	SelectedList;
		HUD_DisplayInfo*		infos;
};

#endif
