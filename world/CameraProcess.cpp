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

#include "CameraProcess.h"
#include "World.h"
#include "CurrentMap.h"
#include "Item.h"
#include "Actor.h"
#include "UCMachine.h"
#include "ShapeInfo.h"
#include "Kernel.h"
#include "CoreApp.h"

// p_dynamic_cast stuff
DEFINE_RUNTIME_CLASSTYPE_CODE(CameraProcess,Process);

// 
// Statics
//
CameraProcess *CameraProcess::camera = 0;

uint16 CameraProcess::SetCameraProcess(CameraProcess *cam)
{
	if (!cam) cam = new CameraProcess();
	if (camera) camera->terminate();
	camera = cam;
	return Kernel::get_instance()->addProcess(camera);
}

void CameraProcess::ResetCameraProcess()
{
	if (camera) camera->terminate();
	camera = 0;
}

void CameraProcess::GetCameraLocation(sint32 &x, sint32 &y, sint32 &z)
{
 	if (!camera) 
	{
		World *world = World::get_instance();
		CurrentMap *map = world->getCurrentMap();
		int map_num = map->getNum();
		Actor* av = world->getNPC(1);
		
		if (!av || av->getMapNum() != map_num)
		{
			x = 8192;
			y = 8192;
			z = 64;
		}
		else
			av->getLocation(x,y,z);
	}
	else
	{
		camera->GetLerped(x, y, z, 256);
	}
}

//
// Constructors
// 

// Track item, do nothing
CameraProcess::CameraProcess(uint16 _itemnum) : 
	time(0), elapsed(0), itemnum(_itemnum), last_framenum(0)
{
	GetCameraLocation(sx,sy,sz);

	if (itemnum)
	{
		Item *item = World::get_instance()->getItem(itemnum);
	
		// Got it
		if (item)
		{
			item->getLocation(ex,ey,ez);
			ez += 20;
		}
		return;
	}

	// No item
	itemnum = 0;
	ex = sx; ey = sy; ez = sz;
}

// Stay over point
CameraProcess::CameraProcess(sint32 _x, sint32 _y, sint32 _z) : 
	ex(_x), ey(_y), ez(_z), time(0), elapsed(0), itemnum(0)
{
	GetCameraLocation(sx,sy,sz);
}

// Scroll
CameraProcess::CameraProcess(sint32 _x, sint32 _y, sint32 _z, sint32 _time) : 
	ex(_x), ey(_y), ez(_z), time(_time), elapsed(0), itemnum(0)
{
	GetCameraLocation(sx,sy,sz);
	pout << "Scrolling from (" << sx << "," << sy << "," << sz << ") to (" <<
		ex << "," << ey << "," << ez << ") in " << time << " frames" << std::endl;
}

bool CameraProcess::run(const uint32 /* framenum */)
{
	if (time && elapsed>time)
	{
		result = 0; // do we need this

		// This is a bit of a hack. If we are looking at avatar,
		// then follow avatar
		/* - Shouldn't be needed. I_setCenterOn(1) is called
		Actor *av = World::get_instance()->getNPC(1);
		sint32 ax, ay, az;
		av->getLocation(ax,ay,az);

		if (ex == ax && ey == ay && ez == (az+20))
		{
			pout << "Hack to scroll with Avatar" << std::endl;
			getGUIInstance()->SetCameraProcess(new CameraProcess(1));
		}
		else 
		*/
		CameraProcess::SetCameraProcess(0);	// This will terminate us
		return false;
	}

	elapsed++;

	return true;
}

void CameraProcess::GetLerped(sint32 &x, sint32 &y, sint32 &z, sint32 factor)
{
	bool inBetween = true;

	uint32 game_framenum = CoreApp::get_instance()->getFrameNum();

	if (game_framenum != last_framenum)
	{
		last_framenum = game_framenum;
		inBetween = false;
	}

	if (time == 0)
	{
		
		if (!inBetween)
		{
			sx = ex;
			sy = ey;
			sz = ez;

			if (itemnum)
			{
				Item *item = World::get_instance()->getItem(itemnum);
				// Got it
				if (item)
				{
					sx = ex;
					sy = ey;
					sz = ez;
					item->getLocation(ex,ey,ez);
					ez += 20;
				}
			}
		}

		if (factor == 256)
		{
			x = ex; y = ey; z = ez;
		}
		else if (factor == 0)
		{
			x = sx; y = sy; z = sz;
		}
		else
		{
			// This way while possibly slower is more accurate
			x = ((sx*(256-factor) + ex*factor)>>8);
			y = ((sy*(256-factor) + ey*factor)>>8);
			z = ((sz*(256-factor) + ez*factor)>>8);
		}
	}
	else
	{
		// Do a quadratic interpolation here of velocity, but not yet
		sint32 sfactor = elapsed;
		sint32 efactor = elapsed+1;

		if (sfactor > time) sfactor = time;
		if (efactor > time) efactor = time;

		sint32 lsx = ((sx*(time-sfactor) + ex*sfactor)/time);
		sint32 lsy = ((sy*(time-sfactor) + ey*sfactor)/time);
		sint32 lsz = ((sz*(time-sfactor) + ez*sfactor)/time);

		sint32 lex = ((sx*(time-efactor) + ex*efactor)/time);
		sint32 ley = ((sy*(time-efactor) + ey*efactor)/time);
		sint32 lez = ((sz*(time-efactor) + ez*efactor)/time);

		// This way while possibly slower is more accurate
		x = ((lsx*(256-factor) + lex*factor)>>8);
		y = ((lsy*(256-factor) + ley*factor)>>8);
		z = ((lsz*(256-factor) + lez*factor)>>8);
	}
}

//	"Camera::move_to(uword, uword, ubyte, word)",
uint32 CameraProcess::I_move_to(const uint8* args, unsigned int /*argsize*/)
{
	ARG_UINT16(x);
	ARG_UINT16(y);
	ARG_UINT8(z);
	ARG_SINT16(unk);
	CameraProcess::SetCameraProcess(new CameraProcess(x,y,z));
	return 0;
}

//	"Camera::setCenterOn(uword)",
uint32 CameraProcess::I_setCenterOn(const uint8* args, unsigned int /*argsize*/)
{
	ARG_UINT16(itemnum);
	CameraProcess::SetCameraProcess(new CameraProcess(itemnum));
	return 0;
}

//	Camera::scrollTo(uword, uword, ubyte, word)
uint32 CameraProcess::I_scrollTo(const uint8* args, unsigned int /*argsize*/)
{
	ARG_UINT16(x);
	ARG_UINT16(y);
	ARG_UINT8(z);
	ARG_SINT16(unk);
	return CameraProcess::SetCameraProcess(new CameraProcess(x,y,z, 15));
}
