#ifndef	__BUILDING_H__
#define __BUILDING_H__

#include <list>
#include "Unit.h"
#include "Production_DB.h"

class	Building : public Unit
{
 private:
	float				_buildingTime;
	std::list<Production_DB*>	_productionList;
 public:
	Building(Unit_Model*, Player* = NULL);
	~Building();
	virtual	void	Update(void);
		void	AddToBuildList(Production_DB*);
		void	SpawnBuiltUnit(int builtID);
};

#endif
