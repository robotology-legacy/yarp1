#include "functionList.h"
#include <iostream>
using namespace std;

extern YARPOutputPortOf<YARPBabyBottle> _outPort;/*(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);*/

// function list
bool addYVector(YARPBabyBottle &bottle)
{
	YVector tmp;
	int n;
	cout << "Adding a YVector\n";
	cout << "Specify length: ";
	cin >> n;
	tmp.Resize(n);
	cout << "Insert now << " << n << " values: ";
	for (int i = 0; i < n; i++)
	{
		cin >> tmp(i+1);
	}

	bottle.writeYVector(tmp);
	return true;
}

bool addYVocab(YARPBabyBottle &bottle)
{
	YBVocab tmp;
	YARPString tmpString;

	cout << "Adding a YBVocab\n";
	cout << "Type verbose command (you may have to check YARPVocab.h): ";
	cin >> tmpString;
	tmp = tmpString;

	bottle.writeVocab(tmp);
	return true;
}

bool addID(YARPBabyBottle &bottle)
{
	YBVocab tmp;
	YARPString tmpString;

	cout << "Adding a YBVocab\n";
	cout << "Type label (you may have to check YARPVocab.h): ";
	cin >> tmpString;
	tmp = tmpString;

	bottle.setID(tmp);
	return true;

}

bool display(YARPBabyBottle &bottle)
{
	cout << "Current bottle is: ";
	bottle.display();

	return true;
}

bool reset(YARPBabyBottle &bottle)
{
	cout << "Resetting bottle";
	bottle.reset();
	cout << "...done!\n";

	return true;
}

bool send(YARPBabyBottle &bottle)
{
	cout << "Sending current bottle:";
	bottle.display();
	_outPort.Content() = bottle;
	_outPort.Write();
	cout << "...done!\n";

	return true;
}

bool addInt(YARPBabyBottle &bottle)
{
	int itmp;
	cout << "Adding an integer\n";
	cout << "Type an integer: ";
	cin >> itmp;
	bottle.writeInt(itmp);
	return true;
}

bool addFloat(YARPBabyBottle &bottle)
{
	double ftmp;
	cout << "Adding a float\n";
	cout << "Type a floating point value: ";
	cin >> ftmp;
	bottle.writeFloat(ftmp);
	return true;
}

bool addText(YARPBabyBottle &bottle)
{
	YBVocab tmp;
	YARPString tmpString;

	cout << "Adding a text string\n";
	cout << "Type string: ";
	cin >> tmpString;
	tmp = tmpString;

	bottle.writeText(tmp.c_str());
	return true;
}

