// repeater.cpp : Defines the entry point for the console application.
//

#include <yarp/YARPPort.h>
#include <yarp/YARPRepeater.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPBabyBottle.h>

#include <iostream>

using namespace std;

extern int __debug_level;

typedef YARPRepeater<YARPBottle> Repeater;

int main(int argc, char* argv[])
{
	// __debug_level = 80;

	YARPString oname;
	YARPString iname;
	YARPString name;
	if (!YARPParseParameters::parse(argc, argv, "-name",  name))
	{
		cout << "Use: " << argv[0] << " --name port_name";
		return 0;
	}
	oname = name+"o";
	iname = name+"i";
	Repeater rep(iname, oname, YARP_UDP, YARP_MCAST);
	rep.Begin();
	
	cout << "q! to exit";
	YARPString c;
	while (cin >> c)
	{
		if (c == "q!")
			break;
	};

	rep.End();

	return 0;
}

