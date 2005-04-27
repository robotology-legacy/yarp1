#include "functionList.h"
#include <iostream>
using namespace std;

extern YARPOutputPortOf<YARPBottle> _outPort;/*(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);*/

/*
// function list
bool addYVector(YARPBottle &bottle)
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
}*/

bool addDoubleVector(YARPBottle &bottle)
{
  double *tmp;
  int n;
  cout << "Adding a YVector\n";
  cout << "Specify length: ";
  cin >> n;
  tmp = new double[n];
  cout << "Insert now << " << n << " values: ";
  for (int i = 0; i < n; i++)
    {
      cin >> tmp[i];
    }

  bottle.writeDoubleVector(tmp,n);
  delete [] tmp;
  return true;
}

bool addIntVector(YARPBottle &bottle)
{
  int *tmp;
  int n;
  cout << "Adding a YVector\n";
  cout << "Specify length: ";
  cin >> n;
  tmp = new int[n];
  cout << "Insert now << " << n << " values: ";
  for (int i = 0; i < n; i++)
    {
      cin >> tmp[i];
    }

  bottle.writeIntVector(tmp,n);
  delete [] tmp;
  return true;
}

/*
bool addYVocab(YARPBottle &bottle)
{
	YBVocab tmp;
	YARPString tmpString;

	cout << "Adding a YBVocab\n";
	cout << "Type verbose command (you may have to check YARPVocab.h): ";
	cin >> tmpString;
	tmp = tmpString;

	bottle.writeVocab(tmp);
	return true;
}*/

bool addID(YARPBottle &bottle)
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

bool display(YARPBottle &bottle)
{
	cout << "Current bottle is: ";
	bottle.display();

	return true;
}

bool reset(YARPBottle &bottle)
{
	cout << "Resetting bottle";
	bottle.reset();
	cout << "...done!\n";

	return true;
}

bool send(YARPBottle &bottle)
{
	cout << "Sending current bottle:";
	bottle.display();
	_outPort.Content() = bottle;
	_outPort.Write();
	cout << "...done!\n";

	return true;
}

bool addInt(YARPBottle &bottle)
{
	int itmp;
	cout << "Adding an integer\n";
	cout << "Type an integer: ";
	cin >> itmp;
	bottle.writeInt(itmp);
	return true;
}

bool addFloat(YARPBottle &bottle)
{
	double ftmp;
	cout << "Adding a float\n";
	cout << "Type a floating point value: ";
	cin >> ftmp;
	bottle.writeFloat(ftmp);
	return true;
}

bool addText(YARPBottle &bottle)
{
	YARPString tmpString;

	cout << "Adding a text string\n";
	cout << "Type string: ";
	cin >> tmpString;

	bottle.writeText(tmpString.c_str());
	return true;
}

