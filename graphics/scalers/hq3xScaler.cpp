//hq3x filter demo program
//----------------------------------------------------------
//Copyright (C) 2003 MaxSt ( maxst@hiend3d.com )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later
//version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "pent_include.h"

#ifdef USE_HQ3X_SCALER

#include "hq3xScaler.h"
#include "Manips.h"
#include "Texture.h"

namespace Pentagram {

hq3xScaler::hq3xScaler() : Scaler()
{
	Scale16Nat = GetScaler<uint16, Manip_Nat2Nat_16, uint16>();
	Scale16Sta = GetScaler<uint16, Manip_Sta2Nat_16, uint32>();

	Scale32Nat = GetScaler<uint32, Manip_Nat2Nat_32, uint32>();
	Scale32Sta = GetScaler<uint32, Manip_Sta2Nat_32, uint32>();
	Scale32_A888 = GetScaler<uint32, Manip_32_A888, uint32>();
	Scale32_888A = GetScaler<uint32, Manip_32_888A, uint32>();
}

const uint32 hq3xScaler::ScaleBits() const { return 1<<3; }
const bool hq3xScaler::ScaleArbitrary() const { return false; }

const char *hq3xScaler::ScalerName() const { return "hq3x"; }
const char *hq3xScaler::ScalerDesc() const { return "hq3x Magnification Filter"; }
const char *hq3xScaler::ScalerCopyright() const { return "Copyright (C) 2003 MaxSt"; }

const hq3xScaler hq3x_scaler;


};	// namespace Pentagram

#endif
