// repeater.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <YARPPort.h>
#include <YARPRepeater.h>
#include <YARPPArseParameters.h>

#include <iostream>
#include <string>

using namespace std;

typedef YARPRepeater<int[2]> Repeater;

int main(int argc, char* argv[])
{
	string oname;
	string iname;
	string name;
	if (!YARPParseParameters::parse(argc, argv, "name",  name))
	{
		cout << "Use: " << argv[0] << " -name port_name";
		return 0;
	}
	oname = name+"o";
	iname = name+"i";
	Repeater rep(iname, oname, YARP_UDP, YARP_MCAST);

	cout << "e to exit";
	char c;
	while(cin>> c)
	{
		if (c == 'e')
			return 0;
	};

	return 0;
}

