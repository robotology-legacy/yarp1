//
// YARPMapCameras.h, part of motorcontrol library.
//

#ifndef __yarpmapcamerash__
#define __yarpmapcamerash__

#include "VisMatrix.h"
#ifdef __QNX__
#include "YARPSafeNew.h"
#endif
#include "YARPBool.h"

// coordinate system as in YARPImageOf<>

const double AF11 = 0.3037;
const double AF22 = 0.4146;
const double T1 = 45.1076;
const double T2 = 37.8712;

class YARPCogMapCameras
{
public:
	static void Foveal2WideangleSquashed (const CVisDVector& src, CVisDVector& dest);
	static void WideangleSquashed2Foveal (const CVisDVector& src, CVisDVector& dest);
	static void Foveal2WideangleSquashed (double sx, double sy, double& dx, double& dy);
	static void WideangleSquashed2Foveal (double sx, double sy, double& dx, double& dy);
};

#endif
