#include "Map_Drawable.h"
#include <iostream>

void	__Map_Drawable__CbClickDown(ID::Control*, ID::Event*, void*)
{
	std::cout << "MD_ClickDown" << std::endl;
}

Map_Drawable::Map_Drawable(int level, uint16_t w, uint16_t h)
{
	this->SetName("Map_Drawable");
	this->_grounds.resize(level);

	this->_w = w;
	this->_h = h;
	this->SetCbClickDown(__Map_Drawable__CbClickDown, NULL);
}

Map_Drawable::~Map_Drawable()
{
	std::cout << "Map_Drawable::~Map_Drawable();\n";
	this->_grounds.clear();
}

void	Map_Drawable::AddGround(ID::Drawable* d, int level)
{
	this->_grounds[level].push_back(d);
	d->SetParent(this);
//	this->SetInvalidate(true);
}

void	Map_Drawable::DelGround(ID::Drawable* d, int level)
{
	std::vector<ID::Drawable*>::iterator	it;
	std::vector<ID::Drawable*>::iterator	end;

	it = this->_grounds[level].begin();
	end = this->_grounds[level].end();
	while (it != end)
	{
		if (*it == d)
		{
			this->_grounds[level].erase(it);
			break ;
		}
		++it;
	}
//	this->SetInvalidate(true);
}

void	Map_Drawable::ClearGround(int level)
{
/*	std::vector<ID::Drawable*>::iterator	it;
	std::vector<ID::Drawable*>::iterator	end;

	it = this->_grounds[level].begin();
	end = this->_grounds[level].end();
	while (it != end)
	{
		(*it)->SetParent(NULL);
		++it;
	}*/
	this->_grounds[level].clear();
	this->SetInvalidate(true);
}

std::vector<ID::Drawable*>*
	Map_Drawable::GetGround(int level)
{
	return &(this->_grounds[level]);
}

int Map_Drawable::RefreshToSurface(ID::Surface* s,
				   int x, int y)
{
	uint32_t				i;
	std::vector<ID::Drawable*>::iterator	it;
	std::vector<ID::Drawable*>::iterator	end;

	i = 0;
	while (i < this->_grounds.size())
	{
		it = this->_grounds[i].begin();
		end = this->_grounds[i].end();
		while (it != end)
		{
			if ((*it)->RefreshToSurface(s, x, y) == -1)
				return -1;
			++it;
		}
		++i;
	}
	this->SetInvalidate(false);
	return 0;
}
