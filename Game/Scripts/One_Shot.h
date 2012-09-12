#ifndef	__ONE_SHOT_H__
#define	__ONE_SHOT_H__

#include "Script_Model.hpp"

class	Bullet_Hit_Model;

class	One_Shot : public Script_Model
{
	public:
				One_Shot();
				~One_Shot();
	//	virtual int	Apply(uint16_t x, uint16_t y, void* data);

	public:
		Bullet_Hit_Model*	bhm;
};

#endif
