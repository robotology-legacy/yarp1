//
// Som.h
//

// c++ wrapper for the Som library.

#ifndef __Somh__
#define __Somh__

// new stuff YARP.
#ifdef __QNX__
#include "YARPSafeNew.h"
#include "YARPBool.h"
#endif

#include "VisMatrix.h"

#include "config.h"
#include "datafile.h"
#include "errors.h"
#include "fileio.h"
#include "labels.h"
#include "lvq_pak.h"
#include "som_rout.h"
#include "version.h"

//
// Self organizing map class.
//
class YARPSom 
{
protected:
	int dimension;
	int xdim, ydim;
	int topol, neigh;
	float alpha1, radius1;
	int fixed, weights;
	float alpha2, radius2;
	
	float *minrange, *maxrange;

	struct entries_tag *data;
	struct entries_tag *codes;
	struct teach_params params;

	long length1, length2, noc;
	struct typelist *type_tmp;
	
	bool initialized;

	int win_x, win_y;
	
public:
	YARPSom ();
	virtual ~YARPSom();

	int  Init (int dim, int width, int height, int len1, float a1, float r1,
			   int len2, float a2, float r2, CVisDVector& min, CVisDVector& max);
	int  Uninit (void);

	// first stage training.
	int  InitTrain1 (void);
	int  Train1 (CVisDVector& s, short weight, int fixx, int fixy);
	int  EndTrain1 (void);

	int  InitTrain2 (void);
	int  Train2 (CVisDVector& s, short weight, int fixx, int fixy);
	int  EndTrain2 (void);

	int  SaveMap (char *filename);
	int  SaveMapPlanes (char *filename);
	int  Serialize (char *filename);   // save complete status.
	int  Unserialize (char *filename); // reload status.

	int  SetSnapshotInfo (char *filename, int interval);
	int  ClearSnapshotInfo (void);

	int  SetVerbosity (int level);	// 0 = no verbosity.

	int  ComputeWinner (CVisDVector& s);
	int  GetWinner (int& x, int& y, CVisDVector& result);

	// used to read the internal data for display or other purposes.
	int  GetSize (int& dimension, int& width, int& height);
	int  GetCodes (float *array);
	int  GetCodes (CVisDVector *array);

	int  GetStep (void) const { if (data != NULL) return data->lap; else return 0; }
};

#endif