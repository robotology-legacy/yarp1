/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
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
/// $Id: main.cpp,v 1.1 2004-07-29 12:26:16 babybot Exp $
///
///


#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/LogPolarSDK.h>

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

	const int len = ACE_OS::strlen(argv[1]);
	if (argv[1][len] != '\\' && argv[1][len] != '/')
		sprintf (Path, "%s/", argv[1]);
	else
		sprintf (Path, "%s", argv[1]);

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

	// DS map 4.0
	Param.padding = _salign;
	Param.Fovea_Type = 0;
	printf ("Creating DS map ...");
	rval = Build_DS_Map (&Param, Path, 4.0);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");
	///////////////////////////////////////

	// DS map 2.0
	Param.padding = _salign;
	Param.Fovea_Type = 0;
	printf ("Creating DS map ...");
	rval = Build_DS_Map (&Param, Path, 2.0);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");
	///////////////////////////////////////

	//// SHIFT MAP 4x
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

	////// SHIFT MAP 2x
	Param = Set_Param(
		_xsize, _ysize,
		256, 256,
		_srho/2, _stheta/2, _sfovea/2,
		1090/2,
		CUST,
		1024.0/1090.0);

	Param.padding = _salign;
	Param.Fovea_Type = 0;
	Param.Ratio = 2.0f;
	Param.dres = 1090.0/2.0;

	printf ("Creating Shift map ...");
	rval = Build_Shift_Map (&Param, Path);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");

	printf ("Creating Step Function ...");

	Build_Step_Function (Path, &Param);
	printf ("\t\tDone !  \n\n");

	//////////

	////// fovea x4
	Param = Set_Param(
		_xsize, _ysize,
		256, 256,
		_srho/4, _stheta/4, _sfovea/4,
		1090/4,
		CUST,
		4*1024.0/1090.0);
	Param.padding = _salign;
	Param.Fovea_Type = 0;
	Param.Ratio = 4.0f;
	Param.dres = 1090.0/4.0;

	printf ("Creating remap map (4x center)...");
	rval = Build_Remap_Map (&Param, Path);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");
	//////////

	////// fovea x2
	Param = Set_Param(
		_xsize, _ysize,
		256, 256,
		_srho/2, _stheta/2, _sfovea/2,
		1090/2,
		CUST,
		2*1024.0/1090.0);
	Param.padding = _salign;
	Param.Fovea_Type = 0;
	Param.Ratio = 2.0f;
	Param.dres = 1090.0/2.0;

	printf ("Creating remap map (2x center)...");
	rval = Build_Remap_Map (&Param, Path);
	if (rval)
		printf ("\t\t\tDone !  \n");
	else 
		printf ("\t\t\tFailed !  \n");
	//////////

	printf("Generation Completed.\n\n");

	return 0;
}

