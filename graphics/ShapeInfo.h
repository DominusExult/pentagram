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

#ifndef SHAPEINFO_H
#define SHAPEINFO_H

class ShapeInfo
{
public:
	enum { SI_FIXED   = 0x0001,
		   SI_SOLID   = 0x0002,
		   SI_SEA     = 0x0004,
		   SI_LAND    = 0x0008,
		   SI_OCCL    = 0x0010,
		   SI_BAG     = 0x0020,
		   SI_DAMAGING= 0x0040,
		   SI_NOISY   = 0x0080,
		   SI_DRAW    = 0x0100,
		   SI_IGNORE  = 0x0200,
		   SI_ROOF    = 0x0400,
		   SI_TRANSL  = 0x0800,
		   SI_EDITOR  = 0x1000,
		   SI_EXPLODE = 0x2000
	} SFlags;

	enum { SF_GENERIC     = 0,
		   SF_QUALITY     = 1,
		   SF_QUANTITY    = 2,
		   SF_GLOBEGG     = 3,
		   SF_UNKEGG      = 4,
		   SF_BREAKABLE   = 5,
		   SF_CONTAINER   = 6,
		   SF_MONSTEREGG  = 7,
		   SF_TELEPORTEGG = 8,
		   SF_REAGENT     = 9
	} SFamily;

	enum { SE_NONE   = 0,
		   SE_SHIELD = 1,
		   SE_ARM    = 2,
		   SE_HEAD   = 3,
		   SE_BODY   = 4,
		   SE_LEGS   = 5,
		   SE_WEAPON = 6
	} SEquipType;
		   

	uint32 flags;
	uint32 x, y, z;
	uint32 family;
	uint32 equiptype;
	uint32 animtype, animdata;
	uint32 weight, volume;

	inline bool is_fixed() const { return (flags & SI_FIXED) != 0; }
	inline bool is_solid() const { return (flags & SI_SOLID) != 0; }
	inline bool is_sea() const { return (flags & SI_SEA) != 0; }
	inline bool is_land() const { return (flags & SI_LAND) != 0; }
	inline bool is_occl() const { return (flags & SI_OCCL) != 0; }
	inline bool is_bag() const { return (flags & SI_BAG) != 0; }
	inline bool is_damaging() const { return (flags & SI_DAMAGING) != 0; }
	inline bool is_noisy() const { return (flags & SI_NOISY) != 0; }
	inline bool is_draw() const { return (flags & SI_DRAW) != 0; }
	inline bool is_ignore() const { return (flags & SI_IGNORE) != 0; }
	inline bool is_roof() const { return (flags & SI_ROOF) != 0; }
	inline bool is_translucent() const { return (flags & SI_TRANSL) != 0; }
	inline bool is_editor() const { return (flags & SI_EDITOR) != 0; }
	inline bool is_explode() const { return (flags & SI_EXPLODE) != 0; }
};


#endif