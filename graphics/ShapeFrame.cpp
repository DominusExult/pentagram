/*
 *  Copyright (C) 2003 The Pentagram Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "pent_include.h"

#include "ShapeFrame.h"
#include "ConvertShape.h"
#include "u8/ConvertShapeU8.h"

/*
  parse data and fill class
 */
ShapeFrame::ShapeFrame(const uint8* data, uint32 size, const ConvertShapeFormat* format) : line_offsets(0)
{
	// Load it as u8
	if (!format || format == &U8ShapeFormat || format == &U82DShapeFormat)
		LoadU8Format(data,size);
	else if (format == &PentagramShapeFormat)
		LoadPentagramFormat(data,size);
	else
		LoadGenericFormat(data,size,format);
}

ShapeFrame::~ShapeFrame()
{
	delete [] line_offsets;
}

// Some macros to make things easier
#define READ1(data,offset) (data[offset])
#define READ2(data,offset) (data[offset] + (data[offset+1] << 8))
#define READ4(data,offset) (data[offset] + (data[offset+1] << 8) + (data[offset+2] << 16) + (data[offset+3] << 24))

// This will load a u8 style shape 'optimzed'.
void ShapeFrame::LoadU8Format(const uint8* data, uint32 size)
{
	compressed = READ1(data,8);
	width = (sint16) READ2(data,10);;
	height = (sint16) READ2(data,12);
	xoff = (sint16) READ2(data,14);
	yoff = (sint16) READ2(data,16);

	if (height == 0) return;

	line_offsets = new uint32[height];

	data += 18;
	for (sint32 i = 0; i < height; i++)
	{
		line_offsets[i] = READ2(data,0) - ((height-i)*2);
		data+=2;
	}

	rle_data = data;
}

// This will load a pentagram style shape 'optimzed'.
void ShapeFrame::LoadPentagramFormat(const uint8* data, uint32 size)
{
	compressed = READ1(data,0);
	width = (sint32) READ4(data,4);
	height = (sint32) READ4(data,8);
	xoff = (sint32) READ4(data,12);
	yoff = (sint32) READ4(data,16);

	if (height == 0) return;

	line_offsets = new uint32[height];

	data += 20;
	for (sint32 i = 0; i < height; i++)
	{
		line_offsets[i] = READ4(data,0);
		data+=4;
	}

	rle_data = data;
}

// This will load any sort of shape via a ConvertShapeFormat struct
void ShapeFrame::LoadGenericFormat(const uint8* data, uint32 size, const ConvertShapeFormat* format)
{
	IBufferDataSource ds(data,size);

	ds.skip(format->bytes_frame_unknown);
	compressed = ds.readX(format->bytes_frame_compression);
	width = ds.readXS(format->bytes_frame_width);
	height = ds.readXS(format->bytes_frame_height);
	xoff = ds.readXS(format->bytes_frame_xoff);
	yoff = ds.readXS(format->bytes_frame_yoff);

	if (height == 0) return;

	line_offsets = new uint32[height];

	if (format->line_offset_absolute) for (sint32 i = 0; i < height; i++) 
	{
		line_offsets[i] = ds.readX(format->bytes_line_offset);
	}
	else for (sint32 i = 0; i < height; i++) 
	{
		line_offsets[i] = ds.readX(format->bytes_line_offset) - ((height-i)*format->bytes_line_offset);
	}

	rle_data = data + format->len_frameheader2 + height*format->bytes_line_offset;
}
