/*
 *  Copyright (C) 2004  Ryan Nunn and The Pentagram Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef SCALER_H_INCLUDED
#define SCALER_H_INCLUDED

#include "Texture.h"
#include "RenderSurface.h"

namespace Pentagram {

/// Base Scaler class
class Scaler
{
protected:
	// Basic scaler function template
	typedef bool (*ScalerFunc) ( Texture *tex, sint32 sx, sint32 sy, sint32 sw, sint32 sh, 
					uint8* pixel, sint32 dw, sint32 dh, sint32 pitch, bool clamp_src);

	//
	// Basic scaler functions (filled in by the scalers constructor)
	//
	ScalerFunc	Scale16Nat;
	ScalerFunc	Scale16Sta;

	ScalerFunc	Scale32Nat;
	ScalerFunc	Scale32Sta;
	ScalerFunc	Scale32_A888;
	ScalerFunc	Scale32_888A;

	Scaler() { }
public:
	//
	// Scaler Capabilites
	//

	virtual const uint32	ScaleBits() = 0;		//< bits for supported integer scaling
	virtual const bool		ScaleArbitrary() = 0;	//< supports arbitrary scaling of any degree 

	virtual const char *	ScalerName() = 0;		//< Name Of the Scaler (1 word)
	virtual const char *	ScalerDesc() = 0;		//< Desciption of the Scaler
	virtual const char *	ScalerCopyright() = 0;	//< Scaler Copyright info

	//
	// Maybe one day... for now we just grab everything from RenderSurface
	// virtual bool	SetDisplayFormat(const RenderSurface::Format &format);

	// Call this to scale a section of the screen
	inline bool Scale(	Texture *texture, sint32 sx, sint32 sy, sint32 sw, sint32 sh, 
						uint8* pixel, sint32 dw, sint32 dh, sint32 pitch, bool clamp_src)
	{
			if (RenderSurface::format.s_bytes_per_pixel == 4) 
			{
				if (texture->format == TEX_FMT_NATIVE || (texture->format == TEX_FMT_STANDARD && 
					RenderSurface::format.a_mask == TEX32_A_MASK && RenderSurface::format.r_mask == TEX32_R_MASK && 
					RenderSurface::format.g_mask == TEX32_G_MASK && RenderSurface::format.b_mask == TEX32_B_MASK))
				{
					if (RenderSurface::format.a_mask == 0xFF000000)
					{
						return Scale32_A888(texture,sx,sy,sw,sh,pixel,dw,dh,pitch,clamp_src);
					}
					else if (RenderSurface::format.a_mask == 0x000000FF)
					{
						return Scale32_888A(texture,sx,sy,sw,sh,pixel,dw,dh,pitch,clamp_src);
					}
					else 
					{
						return Scale32Nat(texture,sx,sy,sw,sh,pixel,dw,dh,pitch,clamp_src);
					}
				}
				else if (texture->format == TEX_FMT_STANDARD)
				{
					return Scale32Sta(texture,sx,sy,sw,sh,pixel,dw,dh,pitch,clamp_src);
				}
			}
			if (RenderSurface::format.s_bytes_per_pixel == 2) 
			{
				if (texture->format == TEX_FMT_NATIVE)
				{
					return Scale16Nat(texture,sx,sy,sw,sh,pixel,dw,dh,pitch,clamp_src);
				}
				else if (texture->format == TEX_FMT_STANDARD)
				{
					return Scale16Sta(texture,sx,sy,sw,sh,pixel,dw,dh,pitch,clamp_src);
				}
			}

		return false;
	}

	virtual ~Scaler() { }
};

}

#endif