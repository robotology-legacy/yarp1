#include "triggerbehavior.h"

const int N = 200;

TBSharedData::TBSharedData()
{
	_outPort.Register("/armtrigger/behavior/o");
	_fixationPort.Register("/armtrigger/fixation/i");
}

TBSharedData::~TBSharedData()
{

}

