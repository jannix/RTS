#include "One_Shot.h"
#include "Actions/Bullet_Hit_Model.h"

One_Shot::One_Shot() : Script_Model((char*)"One Shot",
				    (char*)"Snipe at high speed the head of the target with a bullet.",
				    0,
				    100,
				    72,
				    0,
				    24,
				    7,
				    0)
{
	this->bhm = new Bullet_Hit_Model();
	if (this->bhm == NULL)
		throw 845;
	this->_SetActions(1, this->bhm);
}

One_Shot::~One_Shot()
{
	delete this->bhm;
}

