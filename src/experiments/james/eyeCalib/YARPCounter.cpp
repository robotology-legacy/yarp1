// YARPCounter.cpp: implementation of the YARPCounter class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPCounter.h"
#include "stdio.h"

#define FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )
#define LINE_SIZE 80

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPCounter::YARPCounter()
{
	_size = 0;
	_level = 0;
}

YARPCounter::YARPCounter(int size)
{
	_size = size;
	_level = 0;
}

YARPCounter::~YARPCounter()
{

}

void YARPCounter::reset(int size)
{
	_size = size;
	_level = 0;
}

void YARPCounter::reset()
{
	_level = 0;
}

void YARPCounter::setLevel(int level)
{
	if (_level >= _size)
		return;
	printf("\r[");
	_level = level;
	double percent;
	int chars, spaces, i;
	percent = _level * 100.0 / _size;
	chars = FTOI((LINE_SIZE - 6) * percent / 100.0);
	spaces = LINE_SIZE - 6 - chars;
	for (i=0; i<chars; i++)
		printf("#");
	for (i=0; i<spaces; i++)
		printf(" ");
	if (_level == _size)
		printf("] Ok.\n");
	else	
		printf("] %2d%%", FTOI(percent));
}

void YARPCounter::inc()
{
	if (_level >= _size)
		return;
	printf("\r[");
	_level++;
	double percent;
	int chars, spaces, i;
	percent = _level * 100.0 / _size;
	chars = FTOI((LINE_SIZE - 6) * percent / 100.0);
	spaces = LINE_SIZE - 6 - chars;
	for (i=0; i<chars; i++)
		printf("#");
	for (i=0; i<spaces; i++)
		printf(" ");
	if (_level == _size)
		printf("] Ok.\n");
	else	
		printf("] %2d%%", FTOI(percent));
}
