#include "findHand.h"

using namespace _logpolarParams;

FindHand::FindHand(const std::string &portName):
_inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST),
_outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
_handStatusPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST)
{
	_inPort.Register((portName+"/i:img").c_str());
	_outPort.Register((portName+"/o:img").c_str());
	_handStatusPort.Register((portName+"/i:hand").c_str());

	_actualColored.Resize (_stheta, _srho);
	_actualGrayscale.Resize(_stheta, _srho);
	_oldGrayscale.Resize(_stheta, _srho);
	_difference.Resize(_stheta, _srho);

	_handPosition.Resize(6);
	_handSpeed.Resize(6);

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

	if (_handStatusPort.Read(0))
	{
		memcpy(_handPosition.data(), _handStatusPort.Content().data(), 6*sizeof(double));
		memcpy(_handSpeed.data(), _handStatusPort.Content().data()+6*sizeof(double), 6*sizeof(double));
	}
}
