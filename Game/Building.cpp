#include "Building.h"
#include "../Database/Database.h"
#include "Game.h"
#include "../Constantes.h"
#include "../Network/Protocol_Manager.h"
#include "Player.h"

Building::Building(Unit_Model* Model, Player* p) : Unit(Model, p),
						   _buildingTime(0)
{
	std::cout << "----------------------------Building" << std::endl;
}

Building::~Building()
{
}

void	Building::Update(void)
{
	this->Unit::Update();

	std::list<Production_DB*>::iterator	it;

	it = this->_productionList.begin();
	if (this->_productionList.empty() == false)
	{
		++_buildingTime;
		if (this->_buildingTime == (*it)->prodTime * MAX_TPS)
		{
			this->SpawnBuiltUnit((*it)->prodID);
			this->_buildingTime = 0;
			this->_productionList.pop_front();
		}
	//	std::cout << "Time Left : " <<
	//	((*it)->prodTime) - (this->_buildingTime / MAX_TPS) << std::endl;
	}
}

void	Building::AddToBuildList(Production_DB* PDB)
{
	std::vector<Resource>::iterator	it;
	std::vector<Resource>::iterator	end;

	// Lock mutex game?
	it = PDB->resourcesList.begin();
	end = PDB->resourcesList.end();
	while (it != end)
	{
		if (this->GetPlayer()->GetResource((*it).model).qty < (*it).qty)
		{
			std::cout << "Not enough resources\n";
			return ;
		}
		++it;
	}

	it = PDB->resourcesList.begin();
	while (it != end)
	{
		this->GetPlayer()->AddResource((*it).model, -((*it).qty));
		++it;
	}
	this->_productionList.push_back(PDB);
}

void	Building::SpawnBuiltUnit(int builtID)
{
	Protocol_Manager*	pm;

	pm = Protocol_Manager::GetInstance();

	pm->prot.AddCmdGameCreateUnit(builtID, this->GetPosition().x, this->GetPosition().y, Game::GetInstance()->myID);
}
