// HandThread implementation file


#include "HandThread.h"

HandThread::HandThread(int rate, const char *name):
YARPRateThread(name, rate)
{}

HandThread::~HandThread()
{}
