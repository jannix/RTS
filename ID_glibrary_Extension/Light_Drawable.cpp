#include "Light_Drawable.h"

void	__Light_Drawable__CbClickDown(ID::Control*, ID::Event*, void*)
{
	std::cout << "LD_ClickDown" << std::endl;
}

Light_Drawable::Light_Drawable()
{
	this->SetName("Light Drawable");
	this->SetCbClickDown(__Light_Drawable__CbClickDown, NULL);
	this->SetEnable(true);
}

Light_Drawable::~Light_Drawable()
{
}

int	Light_Drawable::RefreshToSurface(ID::Surface* s,
					 int x, int y)
{
	if (this->_animation.GetSpriteAnime() == NULL)
		return 0;
/*	if (this->_spriteAnime->GetPict()->BlitSurface(this->_visibleRect,
						   s,
						   x + this->_x,
						   y + this->_y) == -1)
*/	if (this->_animation.
		GetSpriteAnime()->
		GetSprite() != NULL &&
	    this->_animation.
		GetSpriteAnime()->
		GetSprite()->
		BlitSurface(&(this->_animation.
			      GetSpriteAnime()->
			      GetRects()[this->_animation.Animate()]),
			    s,
			    x + this->_x,
			    y + this->_y) == -1)
		return -1;
	this->SetInvalidate(false);
	return 0;
}
