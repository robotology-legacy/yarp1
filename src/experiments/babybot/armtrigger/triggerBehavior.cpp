#include "triggerbehavior.h"

const int N = 200;

TBSharedData::TBSharedData()
{
	_outBottlePort.Register("/armtrigger/behavior/o");
}

TBSharedData::~TBSharedData()
{

}

