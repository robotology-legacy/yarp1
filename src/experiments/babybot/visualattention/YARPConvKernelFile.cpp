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
///                           #bremen#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id:
///
///


#include "YARPConvKernelFile.h"


int YARPConvKernelFile::Read(const char *src, YARPConvKernel &dest)
{
	int cols, rows, anchorX, anchorY, ShiftR;
	int* values;

	FILE  *fp = ACE_OS::fopen(src, "rb");
	
	if (!fp)
	{
		ACE_OS::fprintf(stderr, "Error - cannot open file for reading\n");
		return -1;
	}

	// posso utilizzare una struct in modo che sia più veloce???
	ACE_OS::fread(&cols, sizeof(int), 1, fp);
	ACE_OS::fread(&rows, sizeof(int), 1, fp);
	ACE_OS::fread(&anchorX, sizeof(int), 1, fp);
	ACE_OS::fread(&anchorY, sizeof(int), 1, fp);
	ACE_OS::fread(&ShiftR, sizeof(int), 1, fp);
	values = new int [rows*cols];
	ACE_OS::fread(values, sizeof(int), cols*rows, fp);

	dest.Resize(cols, rows, anchorX, anchorY, values, ShiftR);
	
	ACE_OS::fclose(fp);

	return 0;
}


int YARPConvKernelFile::Write(const char *dest, YARPConvKernel &src)
{
	int cols, rows, anchorX, anchorY, ShiftR;
	int *values;
	
	FILE  *fp = ACE_OS::fopen(dest, "wb");
	
	if (!fp)
	{
		ACE_OS::fprintf(stderr, "Error - cannot open file for writing\n");
		return -1;
	}

	src.GetData(cols, rows, anchorX, anchorY, ShiftR);

	values = new int [rows*cols];

	src.GetValues(values);
	
	// posso utilizzare una struct in modo che sia più veloce???
	ACE_OS::fwrite(&cols, sizeof(int), 1, fp);
	ACE_OS::fwrite(&rows, sizeof(int), 1, fp);
	ACE_OS::fwrite(&anchorX, sizeof(int), 1, fp);
	ACE_OS::fwrite(&anchorY, sizeof(int), 1, fp);
	ACE_OS::fwrite(&ShiftR, sizeof(int), 1, fp);
	ACE_OS::fwrite(values, sizeof(int), cols*rows, fp);

	src.Resize(cols, rows, anchorX, anchorY, values, ShiftR);
	
	ACE_OS::fclose(fp);

	delete [] values;
	
	return 0;
}
