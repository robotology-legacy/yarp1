#include "findHand.h"

using namespace _logpolarParams;

FindHand::FindHand(const std::string &portName):
_inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
{
	_inPort.Register((portName+"/img/i").c_str());
	_outPort.Register((portName+"/img/o").c_str());

	_actualColored.Resize (_stheta, _srho);
	_actualGrayscale.Resize(_stheta, _srho);
	_oldGrayscale.Resize(_stheta, _srho);
	_difference.Resize(_stheta, _srho);

	_frame = 0;
}

FindHand::~FindHand()
{

}

void FindHand::Body()
{
	_inPort.Read();
	// save last image
	_actualLp.Refer (_inPort.Content());
	
	// store old img
	_oldGrayscale = _actualGrayscale;
	// reconstruct color
	_mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)_actualLp, _actualColored);
	iplColorToGray(_actualColored, _actualGrayscale);
	iplSubtract(_actualGrayscale, _oldGrayscale, _difference);
	iplThreshold(_difference, _difference, _threshold);

	// send output
	_outPort.Content().Refer(_difference);
	_outPort.Write();

	_frame++;
	if ((_frame%100)==0)
		printf("frame #%5d\r", _frame);
}
