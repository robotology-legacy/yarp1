// repeater.cpp : Defines the entry point for the console application.
//

#include <YARPPort.h>
#include <YARPRepeater.h>
#include <YARPPArseParameters.h>
#include <YARPBottle.h>

#include <iostream>
//#include <string>

using namespace std;

extern int __debug_level;

typedef YARPRepeater<YARPBottle> Repeater;

int main(int argc, char* argv[])
{
	///__debug_level = 80;

	YARPString oname;
	YARPString iname;
	YARPString name;
	if (!YARPParseParameters::parse(argc, argv, "name",  name))
	{
		cout << "Use: " << argv[0] << " -name port_name";
		return 0;
	}
	oname = name+"o";
	iname = name+"i";
	Repeater rep(iname, oname, YARP_UDP, YARP_MCAST);
	rep.Begin();
	
	cout << "e to exit";
	char c;
	while(cin>> c)
	{
		if (c == 'e')
			break;
	};

	rep.End();

	return 0;
}

