/*
Copyright (C) 2003 The Pentagram team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "pent_include.h"

#include "CurrentMap.h"
#include "Map.h"
#include "Item.h"

using std::list; // too messy otherwise
typedef list<Item*> item_list;

CurrentMap::CurrentMap()
	: current_map(0)
{
	items = new list<Item*>*[128]; //! get rid of constants
	for (unsigned int i = 0; i < 128; i++) {
		items[i] = new list<Item*>[128];
	}
}


CurrentMap::~CurrentMap()
{
	clear();

	//! get rid of constants
	for (unsigned int i = 0; i < 128; i++) {
		delete[] items[i];
	}
	delete[] items;
}

void CurrentMap::clear()
{
	// We need to be careful about who exactly deletes the items in a map
	// (CurrentMap or Map)
	// It should probably be CurrentMap, which means a Map has to be
	// emptied when it's loaded into CurrentMap

	//! get rid of constants
	for (unsigned int i = 0; i < 128; i++) {
		for (unsigned int j = 0; j < 128; j++) {
			item_list::iterator iter;
			for (iter = items[i][j].begin(); iter != items[i][j].end(); ++iter)
				delete *iter;
			items[i][j].clear();
		}
	}

	current_map = 0;
}

uint32 CurrentMap::getNum() const
{
	if (current_map == 0)
		return 0;

	return current_map->mapnum;
}

void CurrentMap::writeback()
{
	if (!current_map)
		return;

	//! constants

	for (unsigned int i = 0; i < 128; i++) {
		for (unsigned int j = 0; j < 128; j++) {
			item_list::iterator iter;
			for (iter = items[i][j].begin(); iter != items[i][j].end(); ++iter)
			{
				Item* item = *iter;

				if (item->getExtFlags() || Item::EXT_FIXED) {
					// item came from fixed
					
					current_map->fixeditems.push_back(item);
				} else {
					current_map->dynamicitems.push_back(item);
				}
			}
			items[i][j].clear();
		}
	}
}

void CurrentMap::loadItems(list<Item*> itemlist)
{
	item_list::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); ++iter)
	{
		Item* item = *iter;

		sint32 x,y,z;
		item->getLocation(x,y,z);

		//! constants
		if (x < 0 || y < 0 || x >= 512*128 || y >= 512*128) {
			perr << "Skipping item: out of range (" 
				 << x << "," << y << ")" << std::endl;
			continue;
		}

		sint32 cx, cy;
		cx = x / 512;
		cy = y / 512;

		items[cx][cy].push_back(item);
		item->assignObjId();
	}
}

void CurrentMap::loadMap(Map* map)
{
	current_map = map;

	loadItems(map->fixeditems);
	loadItems(map->dynamicitems);

	// we take control of the items in map, so clear the pointers
	map->fixeditems.clear();
	map->dynamicitems.clear();
}
