#ifndef __FIND_HAND__
#define __FIND_HAND__

#include <YARPLogpolar.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPImages.h>

class FindHand
{
public:
	
	YARPInputPortOf<YARPGenericImage> _inPort;
	YARPOutputPortOf<YARPGenericImage> _outPort;
	YARPInputPortOf<YVector> _handStatusPort;

	YARPGenericImage _actualLp;
	YARPImageOf<YarpPixelBGR> _actualColored;
	YARPImageOf<YarpPixelMono> _actualGrayscale;
	YARPImageOf<YarpPixelMono> _oldGrayscale;
	YARPImageOf<YarpPixelMono> _difference;
	YARPLogpolar _mapper;

	unsigned char _threshold;
	
	FindHand(const std::string &portName = "/handlocalization");
	~FindHand();


	YVector _handSpeed;
	YVector _handPosition;
	void Body();

private:
	int _frame;
};

#endif