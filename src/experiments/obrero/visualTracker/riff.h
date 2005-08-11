/*
* riff.h library for RIFF file format i/o
* Copyright (C) 2000 - 2002 Arne Schirmacher <arne@schirmacher.de>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*
* Tag: $Name: not supported by cvs2svn $
*
* Change log:
* 
* $Log: not supported by cvs2svn $
* Revision 1.5  2004/01/07 17:52:21  ddennedy
* riff fix for 64 OS
*
* Revision 1.4  2003/11/16 22:36:49  ddennedy
* run make pretty and remove deprecated strstream
*
* Revision 1.3  2003/09/17 22:10:12  ddennedy
* major new features: namely interactive mode and AV/C control
*
* Revision 1.2  2003/02/23 02:02:12  ddennedy
* added raw to stdout, updated docs
*
* Revision 1.8.4.1  2002/11/25 04:48:31  ddennedy
* bugfix to report errors when loading files
*
* Revision 1.8  2002/04/21 06:36:40  ddennedy
* kindler avc and 1394 bus reset support in catpure page, honor max file size
*
* Revision 1.7  2002/04/09 06:53:42  ddennedy
* cleanup, new libdv 0.9.5, large AVI, dnd storyboard
*
* Revision 1.3  2002/03/25 21:34:25  arne
* Support for large (64 bit) files mostly completed
*
* Revision 1.2  2002/03/04 19:22:43  arne
* updated to latest Kino avi code
*
* Revision 1.1.1.1  2002/03/03 19:08:08  arne
* import of version 1.01
*
*/

#ifndef _RIFF_H
#define _RIFF_H 1

#include <vector>
using std::vector;

#define QUADWORD signed long long
#define DWORD signed int
#define LONG unsigned int
#define WORD signed short
#define BYTE unsigned char
#define FOURCC unsigned int

#define RIFF_NO_PARENT (-1)
#define RIFF_LISTSIZE (4)
#define RIFF_HEADERSIZE (8)

#ifdef __cplusplus
extern "C"
{
	FOURCC make_fourcc( char * s );
}
#endif

class RIFFDirEntry
{
public:
	FOURCC type;
	FOURCC name;
	off_t length;
	off_t offset;
	int parent;
	int written;

	RIFFDirEntry();
	RIFFDirEntry( FOURCC t, FOURCC n, int l, int o, int p );
};


class RIFFFile
{
public:
	RIFFFile();
	RIFFFile( const RIFFFile& );
	virtual ~RIFFFile();
	RIFFFile& operator=( const RIFFFile& );

	virtual bool Open( const char *s );
	virtual bool Create( const char *s );
	virtual void Close();
	virtual int AddDirectoryEntry( FOURCC type, FOURCC name, off_t length, int list );
	virtual void SetDirectoryEntry( int i, FOURCC type, FOURCC name, off_t length, off_t offset, int list );
	virtual void SetDirectoryEntry( int i, RIFFDirEntry &entry );
	virtual void GetDirectoryEntry( int i, FOURCC &type, FOURCC &name, off_t &length, off_t &offset, int &list ) const;
	virtual RIFFDirEntry GetDirectoryEntry( int i ) const;
	virtual off_t GetFileSize( void ) const;
	virtual void PrintDirectoryEntry( int i ) const;
	virtual void PrintDirectoryEntryData( const RIFFDirEntry &entry ) const;
	virtual void PrintDirectory( void ) const;
	virtual int FindDirectoryEntry( FOURCC type, int n = 0 ) const;
	virtual void ParseChunk( int parent );
	virtual void ParseList( int parent );
	virtual void ParseRIFF( void );
	virtual void ReadChunk( int chunk_index, void *data );
	virtual void WriteChunk( int chunk_index, const void *data );
	virtual void WriteRIFF( void );

protected:
	int fd;

private:
	vector<RIFFDirEntry> directory;
};
#endif
