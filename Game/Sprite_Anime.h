#ifndef __SPRITE_ANIME_H__
#define __SPRITE_ANIME_H__

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "../ID_glibrary.h"

class	Sprite_Anime;

class	Caract_Picture
{
	public:
		Caract_Picture();
		Caract_Picture(double slow, uint8_t a);
		~Caract_Picture();
		void	SetCaract(double slow, uint8_t access);
		void	SetCaract(Caract_Picture* new_caract);
		double	slow;
		uint8_t	access;
};

class	Sprite_Anime
{
	public:
		Sprite_Anime(int ID, char* filename, int frequencyTick, int nb_pict);
		~Sprite_Anime();
		ID::Surface*	GetSprite();
		ID::Rect*	GetRects();
		ID::Color	GetColor();
		int		GetNbPict();
		int		GetFrequency();
		int		GetID();
		ID::Color	color_case;
		Caract_Picture*	caract;
	private:
		ID::Rect*	rects;
		ID::Surface*	sprite;
		int		ID;
		int		frequencyTick;
		int		nb_pict;
};

class	Animation
{
	public:
		Animation(Sprite_Anime* sprite = NULL) : __index(0),
							 __frequencyTick(0),
							 __sprite(sprite)
		{
		}
		~Animation()
		{
		}
		Animation&	operator=(const Animation& a)
		{
			this->__index = a.__index;
			this->__frequencyTick = a.__frequencyTick;
			this->__sprite = a.__sprite;
			return *this;
		}
		int	Animate(void)
		{
			int	ret;

			ret = this->__index;
			++this->__frequencyTick;
			if (__frequencyTick > this->__sprite->GetFrequency())
			{
				++this->__index;
				if (this->__index >= this->__sprite->GetNbPict())
					this->__index = 0;
				this->__frequencyTick = 0;
			}
			return ret;
		}
		inline void	SetSpriteAnime(Sprite_Anime* sprite)
		{
			this->__sprite = sprite;
		}
		inline Sprite_Anime*	GetSpriteAnime(void) const
		{
			return this->__sprite;
		}

	private:
		int		__index;
		int		__frequencyTick;
		Sprite_Anime*	__sprite;
};

#endif
