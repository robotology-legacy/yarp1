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
///                    #fberton, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: main.cpp,v 1.17 2004-02-04 16:42:09 babybot Exp $
///
///


#include <stdio.h>
#include "LogPolarSDK.h"

/// as copied from YARPLogpolar.h
///

namespace _logpolarParams
{
	const int _xsize = 256;
	const int _ysize = 256;
	const int _srho = 152;
	const int _stheta = 252;
	const int _sfovea = 42;
	const int _salign = 8;
};

///
///
int main (int argc, char *argv[])
{
	using namespace _logpolarParams;

	char Path[512];

	int rval;

	if (argc < 2)
	{
		printf ("Use %s <path> (with trailing backslash!)\n", argv[0]);
		return -1;
	}

	sprintf (Path, "%s\0", argv[1]);
	printf ("Creating maps in : %s\n", Path);

	Image_Data Param = Set_Param(
		_xsize, _ysize,
		256, 256,
		_srho, _stheta, _sfovea,
		1090,
		CUST,
		256.0/1090.0);
	
	Param.padding = _salign;
	Param.Fovea_Type = 0;

	printf ("Creating Angular Shift map ...");
	rval = Build_Ang_Shift_Map(&Param, Path);
	if (rval)
		printf ("\t\tDone !  \n");
	else 
		printf ("\t\tFailed !  \n");

	printf ("Creating Pad map ...");
	rval = Build_Pad_Map(&Param, Path);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");

	printf ("Creating Remap map (Whole Image) ...", Path);
	rval = Build_Remap_Map (&Param, Path);
	if (rval)
		printf ("\tDone !  \n");
	else 
		printf ("\tFailed !  \n");

	
	printf ("Creating Cart2LP map ...", Path);
	rval = Build_Cart2LP_Map(&Param, Path);
	if (rval)
		printf ("\t\tDone !  \n");
	else 
		printf ("\t\tFailed !  \n");

	Param = Set_Param(
		_xsize, _ysize,
		128, 128,
		_srho, _stheta, _sfovea,
		1090,
		CUST,
		512.0/1090.0);

	Param.padding = _salign;
	Param.Fovea_Type = 0;

	printf ("Creating Remap map (Center) ...");
	rval = Build_Remap_Map(&Param, Path);
	if (rval)
		printf ("\t\tDone !  \n");
	else 
		printf ("\t\tFailed !  \n");

	printf ("Creating Color map ...");
	rval = Build_Color_Map(&Param,Path);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");


	///
	Param.Pix_Numb = 2;

	printf ("Creating XY map ...");
	rval = Build_XY_Map(&Param, Path);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");

	printf ("Creating Neigborhood map ...");
	rval = Build_Neighborhood_Map(&Param, Path);
	if (rval)
		printf ("\t\tDone !  \n");
	else 
		printf ("\t\tFailed !  \n");

	printf ("Creating Weights map ...");
	rval = Build_Weights_Map(&Param, Path);	
	if (rval)
		printf ("\t\tDone !  \n");
	else 
		printf ("\t\tFailed !  \n");


	Param.padding = _salign;
	Param.Fovea_Type = 0;
	printf ("Creating DS map ...");
	rval = Build_DS_Map (&Param, Path, 4.0);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");


	Param = Set_Param(
		_xsize, _ysize,
		256, 256,
		_srho/4, _stheta/4, _sfovea/4,
		1090/4,
		CUST,
		1024.0/1090.0);
	Param.padding = _salign;
	Param.Fovea_Type = 0;
	Param.Ratio = 4.0f;
	Param.dres = 1090.0/4.0;

	printf ("Creating Shift map ...");
	rval = Build_Shift_Map (&Param, Path);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");

	printf ("Creating Step Function ...");

	Build_Step_Function (Path, &Param);
	printf ("\t\tDone !  \n\n");

	printf("Generation Completed.\n\n");

	return 0;
}

