/*
Copyright (C) 2003-2005 The Pentagram team

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

#include "SavegameWriter.h"
#include "ODataSource.h"

// zip API
#include "zip.h"

// ioapi ODataSource wrapper functions

static voidpf ods_open(voidpf opaque, const char* filename, int mode);
static uLong ods_read(voidpf opaque, voidpf stream, void* buf, uLong size);
static uLong ods_write(voidpf opaque, voidpf stream,
					   const void* buf, uLong size);
static long ods_tell(voidpf opaque, voidpf stream);
static long ods_seek(voidpf opaque, voidpf stream, uLong offset, int origin);
static int ods_close(voidpf opaque, voidpf stream);
static int ods_error(voidpf opaque, voidpf stream);

zlib_filefunc_def ODS_filefunc_templ = {
	ods_open, ods_read, ods_write, ods_tell, ods_seek, ods_close, ods_error, 0
};



SavegameWriter::SavegameWriter(ODataSource* ds_)
{
	ds = ds_;

	zlib_filefunc_def filefuncs = ODS_filefunc_templ;
	filefuncs.opaque = static_cast<void*>(ds);

	zipFile zfile = zipOpen2("", 0, 0, &filefuncs);
	zipfile = static_cast<void*>(zfile);
}

SavegameWriter::~SavegameWriter()
{
	if (ds)
		delete ds;
	ds = 0;
}

bool SavegameWriter::finish()
{
	zipFile zfile = static_cast<zipFile>(zipfile);
	zipfile = 0;
	if (zipClose(zfile, comment.c_str()) != ZIP_OK) return false;

	return true;
}


bool SavegameWriter::writeFile(const char* name,
							   const uint8* data, uint32 size)
{
	zipFile zfile = static_cast<zipFile>(zipfile);
	perr << name << ": " << size << std::endl;

	if (zipOpenNewFileInZip(zfile, name, 0, 0, 0, 0, 0, 0,
							Z_DEFLATED, Z_BEST_COMPRESSION) != ZIP_OK)
		return false;
	
	if (zipWriteInFileInZip(zfile, data, size) != ZIP_OK)
		return false;

	if (zipCloseFileInZip(zfile) != ZIP_OK)
		return false;

	return true;
}

bool SavegameWriter::writeFile(const char* name, OAutoBufferDataSource* ods)
{
	return writeFile(name, ods->getBuf(), ods->getSize());
}

bool SavegameWriter::writeVersion(uint32 version)
{
	uint8 buf[4];
	buf[0] = version & 0xFF;
	buf[1] = (version >> 8) & 0xFF;
	buf[2] = (version >> 16) & 0xFF;
	buf[3] = (version >> 24) & 0xFF;
	return writeFile("VERSION", buf, 4);
}

bool SavegameWriter::writeDescription(const std::string& desc)
{
	comment = desc;
	return true;
}


// ------------

static voidpf ods_open(voidpf opaque, const char* filename, int mode)
{
	// write-only, for now
//	if (mode != (ZLIB_FILEFUNC_MODE_WRITE | ZLIB_FILEFUNC_MODE_CREATE))
//		return 0;

	// opaque is actually the ODataSource*
	return opaque;
}

static uLong ods_read(voidpf opaque, voidpf stream, void* buf, uLong size)
{
	return 0;
}

static uLong ods_write(voidpf opaque, voidpf stream,
					   const void* buf, uLong size)
{
	ODataSource* ods = static_cast<ODataSource*>(stream);
	ods->write(buf, size);
	return size;
}

static long ods_tell(voidpf opaque, voidpf stream)
{
	ODataSource* ods = static_cast<ODataSource*>(stream);
	return ods->getPos();
}

static long ods_seek(voidpf opaque, voidpf stream, uLong offset, int origin)
{
	ODataSource* ods = static_cast<ODataSource*>(stream);
	switch (origin) {
	case ZLIB_FILEFUNC_SEEK_CUR:
		ods->skip(offset);
		break;
	case ZLIB_FILEFUNC_SEEK_END:
		ods->seek(ods->getSize()+offset);
		break;
	case ZLIB_FILEFUNC_SEEK_SET:
		ods->seek(offset);
		break;
	default:
		return -1;
	}
	return 0;
}

static int ods_close(voidpf opaque, voidpf stream)
{
	return 0;
}

static int ods_error(voidpf opaque, voidpf stream)
{
	return 0;
}

