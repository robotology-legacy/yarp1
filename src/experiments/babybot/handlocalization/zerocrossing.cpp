#include "zerocrossing.h"
#include <iostream>

using namespace std;

ZeroCrossing::ZeroCrossing(double t)
{
	///////////
	_threshold = t;
	reset();
}

ZeroCrossing::~ZeroCrossing()
{
}

void ZeroCrossing::dump()
{/*
	int i;
	for(i = 0; i < _size; i++)
	{
		cout << "--->"<< (i+1) << "\n";
		vector<int>::iterator it;
		if (!_measures[i].empty())
			for(it=_measures[i].begin(); it != _measures[i].end(); it++)
			{
				cout << (*it) << '\t';
			}
		cout << endl;
	
	}
*/
}

void ZeroCrossing:: find(double in, double *out)
{
	_index++;
	*out = 0;
	if (_idle)
	{
		if (in > _threshold)
		{
			*out = 1;
			if (_last != 0)
			{
				_measure += (_index - _last);
			}
				
			_last = _index;
			_idle = false;
			_rise = true;
		}
	}
	else if (_rise)
	{
		if (in < _threshold)
		{
			_idle = true;
			_rise = false;
		}
	}
}
