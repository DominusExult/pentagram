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
#include "Actor.h"

#include "Kernel.h"
#include "UCMachine.h"
#include "World.h"
#include "ActorAnimProcess.h"

// p_dynamic_cast stuff
DEFINE_DYNAMIC_CAST_CODE(Actor,Container);

Actor::Actor()
	: animating(false), strength(0), dexterity(0), intelligence(0),
	  hitpoints(0), mana(0), lastanim(0), direction(0)
{

}

Actor::~Actor()
{

}

uint32 Actor::I_isNPC(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	if (!actor) return 0;
	return 1;
}

uint32 Actor::I_getMap(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	if (!actor) return 0;

	return actor->getMapNum();
}


uint32 Actor::I_doAnim(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	ARG_UINT16(anim);
	ARG_UINT16(dir); // seems to be 1-8
	ARG_UINT16(unk1); // this is almost always 10000 in U8.Maybe speed-related?
	ARG_UINT16(unk2); // appears to be 0 or 1. Some flag?

	if (!actor) return 0;

	if (dir < 1 || dir > 8) {
		perr << "Actor::doAnim: Invalid direction (" << dir << ")" <<std::endl;
		return 0;
	}

	Process *p = new ActorAnimProcess(actor, anim, dir-1);

	return Kernel::get_instance()->addProcess(p);
}

uint32 Actor::I_getDir(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	if (!actor) return 0;

	return actor->getDir();
}

uint32 Actor::I_getLastAnimSet(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	if (!actor) return 0;

	return actor->getLastAnim();
}

uint32 Actor::I_getStr(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	if (!actor) return 0;

	return actor->getStr();
}

uint32 Actor::I_getDex(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	if (!actor) return 0;

	return actor->getDex();
}

uint32 Actor::I_getInt(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	if (!actor) return 0;

	return actor->getInt();
}

uint32 Actor::I_getHp(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	if (!actor) return 0;

	return actor->getHP();
}

uint32 Actor::I_getMana(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	if (!actor) return 0;

	return actor->getMana();
}

uint32 Actor::I_setStr(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	ARG_UINT16(str);
	if (!actor) return 0;

	actor->setStr(str);
	return 0;
}

uint32 Actor::I_setDex(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	ARG_UINT16(dex);
	if (!actor) return 0;

	actor->setDex(dex);
	return 0;
}

uint32 Actor::I_setInt(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	ARG_UINT16(int_);
	if (!actor) return 0;

	actor->setStr(int_);
	return 0;
}

uint32 Actor::I_setHp(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	ARG_UINT16(hp);
	if (!actor) return 0;

	actor->setHP(hp);
	return 0;
}

uint32 Actor::I_setMana(const uint8* args, unsigned int /*argsize*/)
{
	ARG_ACTOR(actor);
	ARG_UINT16(mp);
	if (!actor) return 0;

	actor->setMana(mp);
	return 0;
}

