// bottlesender.cpp : Defines the entry point for the console application.
//
// -- by nat
// allow to build and send bottles to specific targets
// 
// usage:
// command line:
// -port /port/i:bot	--> define port name to listen to, , 
//							

#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPParseParameters.h>

#include <iostream>
using namespace std;

YARPInputPortOf<YARPBottle> _inPort;

int main(int argc, char* argv[])
{
  _inPort.Register("/bottledumper/i:bot");
  while(true)
    {
      _inPort.Read();
      _inPort.Content().display();
    }
  return 0;
}
