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

#ifndef AVATARMOVERPROCESS_H
#define AVATARMOVERPROCESS_H

#include "Process.h"

class AvatarMoverProcess : public Process
{
public:
	AvatarMoverProcess();
	virtual ~AvatarMoverProcess();

	// p_dynamic_cast stuff
	ENABLE_RUNTIME_CLASSTYPE();

	virtual bool run(const uint32 framenum);

	void OnMouseDown(int button, int mx, int my);
	void OnMouseUp(int button);

	bool loadData(IDataSource* ids);
protected:
	virtual void saveData(ODataSource* ods);

	uint32 lastframe;

	uint32 lastMouseDown[2];
	int mouseState[2];
	enum MouseButtonState {
		MBS_DOWN = 0x1,
		MBS_HANDLED = 0x2
	};
};



#endif