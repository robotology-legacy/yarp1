#ifndef __FIND_HAND__
#define __FIND_HAND__

#include <YARPLogpolar.h>
#include <YARPPort.h>
#include <YARPImages.h>

class FindHand
{
public:
	
	YARPInputPortOf<YARPGenericImage> _inPort;
	
	YARPOutputPortOf<YARPGenericImage> _outPort;

	YARPGenericImage _actualLp;
	YARPImageOf<YarpPixelBGR> _actualColored;
	YARPImageOf<YarpPixelMono> _actualGrayscale;
	YARPImageOf<YarpPixelMono> _oldGrayscale;
	YARPImageOf<YarpPixelMono> _difference;
	YARPLogpolar _mapper;

	unsigned char _threshold;
	
	FindHand(const std::string &portName);
	~FindHand();


	void Body();

private:
	int _frame;
};

#endif