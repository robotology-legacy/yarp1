/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPVectorPortContent.h,v 1.6 2003-07-22 15:14:32 babybot Exp $
///
///

#ifndef YARPVECTORPORTCONTENT_INC
#define YARPVECTORPORTCONTENT_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include "YARPMath.h"
#include "YARPPort.h"
#include "YARPPortContent.h"

#include "begin_pack_for_net.h"

class YARPVectorPortContentHeader
{
public:
  NetInt32 len;
} PACKED_FOR_NET;

class YARPMatrixPortContentHeader
{
public:
  NetInt32 r;
  NetInt32 c;
} PACKED_FOR_NET;

#include "end_pack_for_net.h"

class YARPVectorPortContent : public YVector, public YARPPortContent
{
public:
	YARPVectorPortContentHeader header;

	virtual ~YARPVectorPortContent () {}

	//// of course this only works on machines w/ the same floating point representation.
	virtual int Read (YARPPortReader& reader)
	{
		if (reader.Read ((char*)(&header), sizeof(header)))
		{
			if (header.len != Length()) 
				Resize (header.len);

			char *mem = (char *)data();
			ACE_ASSERT (mem != NULL);
			reader.Read (mem, header.len * sizeof(double));
		}
		return 1;
	}

	virtual int Write (YARPPortWriter& writer)
	{
		header.len = Length();
		writer.Write ((char*)(&header), sizeof(header));

		char *mem = (char *)data();
		ACE_ASSERT (mem != NULL);
		writer.Write (mem, header.len * sizeof(double));
		return 1;
	}

	// Called when communications code is finished with the object, and
	// it will be passed back to the user.
	// Often fine to do nothing here.
	virtual int Recycle() { return 0; }

    YVector& operator=(const YVector &vec) { return YVector::operator= (vec); }
};


class YARPInputPortOf<YVector> : public YARPBasicInputPort<YARPVectorPortContent>
{
public:
	YARPInputPortOf<YVector>(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicInputPort<YARPVectorPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPInputPortOf<YVector> () { ((Port *)system_resource)->End(); }
};

class YARPOutputPortOf<YVector> : public YARPBasicOutputPort<YARPVectorPortContent>
{
public:
	YARPOutputPortOf<YVector>(int n_service_type = DEFAULT_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicOutputPort<YARPVectorPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPInputPortOf<YVector> () { ((Port *)system_resource)->End(); }
};

class YARPMatrixPortContent : public YMatrix, public YARPPortContent
{
public:
	YARPMatrixPortContentHeader header;

	virtual ~YARPMatrixPortContent () {}

	//// of course this only works on machines w/ the same floating point representation.
	virtual int Read (YARPPortReader& reader)
	{
		if (reader.Read ((char*)(&header), sizeof(header)))
		{
			if (header.r != NRows() || header.c != NCols())
				Resize (header.r, header.c);

			char *mem = (char *)(data()[0]);		/// mem is contiguous.
			ACE_ASSERT (mem != NULL);
			reader.Read (mem, header.r * header.c * sizeof(double));
		}
		return 1;
	}

	virtual int Write (YARPPortWriter& writer)
	{
		header.r = NRows();
		header.c = NCols();
		writer.Write ((char*)(&header), sizeof(header));

		char *mem = (char *)(data()[0]);
		ACE_ASSERT (mem != NULL);
		writer.Write (mem, header.r * header.c * sizeof(double));
		return 1;
	}

	// Called when communications code is finished with the object, and
	// it will be passed back to the user.
	// Often fine to do nothing here.
	virtual int Recycle() { return 0; }

    YMatrix& operator=(const YMatrix &mat) { return YMatrix::operator= (mat); }
};


class YARPInputPortOf<YMatrix> : public YARPBasicInputPort<YARPMatrixPortContent>
{
public:
	YARPInputPortOf<YMatrix>(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicInputPort<YARPMatrixPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPInputPortOf<YMatrix> () { ((Port *)system_resource)->End(); }
};

class YARPOutputPortOf<YMatrix> : public YARPBasicOutputPort<YARPMatrixPortContent>
{
public:
	YARPOutputPortOf<YMatrix>(int n_service_type = DEFAULT_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicOutputPort<YARPMatrixPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPOutputPortOf<YMatrix> () { ((Port *)system_resource)->End(); }
};


#endif
