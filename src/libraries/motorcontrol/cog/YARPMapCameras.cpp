//
// YARPMapCameras.cpp
//

#include <YARPMapCameras.h>


void YARPCogMapCameras::Foveal2WideangleSquashed (const CVisDVector& src, CVisDVector& dest)
{
	dest(1) = src(1) * AF11 + T1;
	dest(2) = src(2) * AF22 + T2;

	if (dest(1) >= 128) dest(1) = 127;
	if (dest(1) < 0) dest(1) = 0;
	if (dest(2) >= 128) dest(2) = 127;
	if (dest(2) < 0) dest(2) = 0;
}

void YARPCogMapCameras::WideangleSquashed2Foveal (const CVisDVector& src, CVisDVector& dest)
{
	dest(1) = (src(1) - T1) / AF11;
	dest(2) = (src(2) - T2) / AF22;

	if (dest(1) >= 128) dest(1) = 127;
	if (dest(1) < 0) dest(1) = 0;
	if (dest(2) >= 128) dest(2) = 127;
	if (dest(2) < 0) dest(2) = 0;
}

void YARPCogMapCameras::Foveal2WideangleSquashed (double sx, double sy, double& dx, double& dy)
{
	dx = sx * AF11 + T1;
	dy = sy * AF22 + T2;

	if (dx >= 128) dx = 127;
	if (dx < 0) dx = 0;
	if (dy >= 128) dy = 127;
	if (dy < 0) dy = 0;
}

void YARPCogMapCameras::WideangleSquashed2Foveal (double sx, double sy, double& dx, double& dy)
{
	dx = (sx - T1) / AF11;
	dy = (sy - T2) / AF22;

	if (dx >= 128) dx = 127;
	if (dx < 0) dx = 0;
	if (dy >= 128) dy = 127;
	if (dy < 0) dy = 0;
}
