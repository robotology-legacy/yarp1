// repeater.cpp : Defines the entry point for the console application.
//

#include <yarp/YARPPort.h>
#include <yarp/YARPRepeater.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPBabyBottle.h>

#include <iostream>

using namespace std;

extern int __debug_level;

typedef YARPRepeater<YARPBabyBottle> Repeater;

int main(int argc, char* argv[])
{
	///set_yarp_debug (100, 100);

	YARPString oname;
	YARPString iname;
	YARPString name;
	if (!YARPParseParameters::parse(argc, argv, "-name",  name))
	{
		cout << "Use: " << argv[0] << " --name port_name";
		return 0;
	}
	oname = name+"/o";
	iname = name+"/i";
	Repeater rep(iname, oname, YARP_UDP, YARP_MCAST);
	rep.Begin();
	
	cout << "q! to exit\n";
	YARPString c;
	while (cin >> c)
	{
		if (c == "q!")
			break;
	};

	rep.End();

	return 0;
}

