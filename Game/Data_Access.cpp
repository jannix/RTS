#include "Data_Access.h"
#include "Sprite_Anime.h"
#include "../Constantes.h"

Data_Access::Data_Access()
{
}

Data_Access::~Data_Access()
{
	std::vector<Sprite_Anime*>::iterator	itM;
	std::vector<Sprite_Anime*>::iterator	endM;
	std::vector<Sprite_Anime*>::iterator	itU;
	std::vector<Sprite_Anime*>::iterator	endU;

	itM = this->__mapPictures.begin();
	endM = this->__mapPictures.end();
	while (itM != endM)
	{
		delete *itM;
		++itM;
	}
	itU = this->__unitPictures.begin();
	endU = this->__unitPictures.end();
	while (itU != endU)
	{
		delete *itU;
		++itU;
	}
}

int	Data_Access::Init()
{
	Caract_Picture	caract;
	ID::Color	color(50, 54, 67, 0);

	caract.SetCaract(1.00, 1);
	if (-1 == this->LoadMapPicture((char*)"Resources/Sprites/Maps/unit2.bmp", &caract, &color, 3))
		return -1;
	if (-1 == this->LoadMapPicture((char*)"Resources/Sprites/Maps/rock.bmp", &caract, &color, 5))
		return -1;
	color.r = 0;
	color.g = 255;
	color.b = 0;
	color.a = 0;
	if (-1 == this->LoadMapPicture((char*)"Resources/Sprites/Maps/mud.bmp", &caract, &color, 0))
		return -1;
	if (-1 == this->LoadMapAnime((char*)"Resources/Sprites/Maps/mud_a.bmp", 0, 0, 160, 32, 5, &caract, MAX_TPS / 4, 4))
		return -1;
	caract.SetCaract(1.00, 1);
	if (-1 == this->LoadMapPicture((char*)"Resources/Sprites/Maps/mud1.bmp", &caract, &color, 1))
		return -1;
	caract.SetCaract(1.00, 1);
	if (-1 == this->LoadMapPicture((char*)"Resources/Sprites/Maps/mud2.bmp", &caract, &color, 6))
		return -1;
	color.r = 255;
	color.g = 255;
	color.b = 0;
	color.a = 0;
	if (-1 == this->LoadMapPicture((char*)"Resources/Sprites/Maps/sable1.bmp", &caract, &color, 7))
		return -1;
	if (-1 == this->LoadMapAnime((char*)"Resources/Sprites/Maps/none.bmp", 0, 0, 40, 10, 4, &caract, MAX_TPS / 4, 8))
		return -1;
	caract.SetCaract(1.00, 0);
	color.r = 0;
	color.g = 0;
	color.b = 255;
	color.a = 0;
	if (-1 == this->LoadMapPicture((char*)"Resources/Sprites/Maps/water.bmp", &caract, &color, 9))
		return -1;
	caract.SetCaract(1.00, 0);
	if (-1 == this->LoadMapPicture((char*)"Resources/Sprites/Maps/water_t.bmp", &caract, &color, 10))
		return -1;
	caract.SetCaract(1.00, 0);
	if (-1 == this->LoadMapPicture((char*)"Resources/Sprites/Maps/water_b.bmp", &caract, &color, 11))
		return -1;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 0;
	if (this->LoadMapPicture((char*)"Resources/Sprites/Maps/nuage.bmp",
				 &caract, &color, 12) == -1)
		return -1;

	if (this->LoadUnitPicture((char*)"Resources/Sprites/Units/1.bmp",
				 &caract, &color, 1) == -1)
		return -1;
	if (this->LoadUnitPicture((char*)"Resources/Sprites/Units/2.bmp",
				 &caract, &color, 2) == -1)
		return -1;
	if (this->LoadUnitPicture((char*)"Resources/Sprites/Units/3.bmp",
				 &caract, &color, 3) == -1)
		return -1;
	return 0;
}


Sprite_Anime*	Data_Access::GetMapPicture(int ID)
{
	std::vector<Sprite_Anime*>::iterator	it;
	std::vector<Sprite_Anime*>::iterator	end;

	it = this->__mapPictures.begin();
	end = this->__mapPictures.end();
	while (it != end)
	{
		if ((*it)->GetID() == ID)
			return *it;
		++it;
	}
	return NULL;
}

Sprite_Anime*	Data_Access::GetUnitPicture(int ID)
{
	std::vector<Sprite_Anime*>::iterator	it;
	std::vector<Sprite_Anime*>::iterator	end;

	it = this->__unitPictures.begin();
	end = this->__unitPictures.end();
	while (it != end)
	{
		if ((*it)->GetID() == ID)
			return *it;
		++it;
	}
	return NULL;
}

int	Data_Access::LoadMapPicture(char* str,
				    Caract_Picture* caract,
				    ID::Color* color,
				    int ID)
{
	Sprite_Anime*	tmp;

	tmp = this->__CreateSpritePicture(str, caract, color, ID);
	if (tmp == NULL)
		return -1;
	this->__mapPictures.push_back(tmp);
	return 0;
}

int	Data_Access::LoadUnitPicture(char* str,
				     Caract_Picture* caract,
				     ID::Color* color,
				     int ID)
{
	Sprite_Anime*	tmp;

	tmp = this->__CreateSpritePicture(str, caract, color, ID);
	if (tmp == NULL)
		return -1;
	this->__unitPictures.push_back(tmp);
	return 0;
}

int	Data_Access::LoadMapAnime(char* str,
				  int x, int y,
				  int w, int h,
				  int nb_frames,
				  Caract_Picture* caract,
				  int frequency,
				  int ID)
{
	Sprite_Anime*	sprite;

	sprite = this->__CreateSpriteAnime(str, x, y, w, h, nb_frames, caract, frequency, ID);
	if (sprite == NULL)
		return -1;
	this->__mapPictures.push_back(sprite);
	return 0;
}

int	Data_Access::LoadUnitAnime(char* str,
				  int x, int y,
				  int w, int h,
				  int nb_frames,
				  Caract_Picture* caract,
				  int frequency,
				  int ID)
{
	Sprite_Anime*	sprite;

	sprite = this->__CreateSpriteAnime(str, x, y, w, h, nb_frames, caract, frequency, ID);
	if (sprite == NULL)
		return -1;
	this->__unitPictures.push_back(sprite);
	return 0;
}

Sprite_Anime*	Data_Access::__CreateSpritePicture(char* str,
						   Caract_Picture* caract,
						   ID::Color* color,
						   int ID)
{
	Sprite_Anime*	tmp;

	tmp = new Sprite_Anime(ID, str, 0, 1);
	tmp->color_case = *color;
	tmp->caract = new Caract_Picture(caract->slow, caract->access);
	tmp->GetRects()[0].x = 0;
	tmp->GetRects()[0].y = 0;
	tmp->GetRects()[0].w = tmp->GetSprite()->w;
	tmp->GetRects()[0].h = tmp->GetSprite()->h;
	return tmp;
}

Sprite_Anime*	Data_Access::__CreateSpriteAnime(char* str,
						 int x, int y,
						 int w, int h,
						 int nb_frames,
						 Caract_Picture* caract,
						 int frequency,
						 int ID)
{
	Sprite_Anime*	new_anime;
	int		i;
	ID::Rect*	rects;

	new_anime = new Sprite_Anime(ID, str, frequency, nb_frames);
	new_anime->caract = new Caract_Picture(caract->slow, caract->access);
	rects = new_anime->GetRects();
	i = 0;
	while (i < nb_frames)
	{
		rects[i].x = x + i * w / nb_frames;
		rects[i].y = y;
		rects[i].w = w / nb_frames;
		rects[i].h = h;
		++i;
	}
	return new_anime;
}

