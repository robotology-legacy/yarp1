// bottlesender.cpp : Defines the entry point for the console application.
//
// -- by nat
// allow to build and send bottles to specific targets
// 
// usage:
// -interactive		--> interactive mode, connect the port manually and follow the menu
// -basename	--> (interactive mode) the port would register output port as /basename/o
//
// command line:
// -port /remoteport/i:bot	--> define remote port name to connect to, , 
//							--> this is mandatory, the the prg will connect/disc automatically
// to build the bottle use one or more:
// -label mystring			--> set mystring as the bottle's label
// -text mystring			--> put text in the bottle
// -vocab mystring			--> store a string as a vocab
// -int 10					--> store 10 as an integer
// -float 0.2				--> store 0.2 as a float
// 

#include <yarp/YARPPort.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPConfigRobot.h>

#include "functionList.h"

#include <iostream>
using namespace std;

const char *DEFAULT_NAME = "/bottlesender";
void interactive(int argc, char *argv[]);
void command(int argc, char *argv[]);
enum __ERROR_CODE {
	CONNECT, 
	PARAMETERS, 
	GENERIC, 
	PARSE_BOTTLE};
typedef __ERROR_CODE ERROR_CODE;

void error(ERROR_CODE err);
void printMenu();

/*static */ YARPOutputPortOf<YARPBabyBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
COMMAND_TABLE _commands;

void _fillTable()
{
	// global commands
	REG_CMD(addYVector, "YVector", "put an YVector into the bottle");
	REG_CMD(addYVocab, "YBVocab", "put an YBVocab into the bottle");
	REG_CMD(addID, "ID", "set bottle id");
	REG_CMD(addText, "Text", "put an text string into the bottle");
	REG_CMD(addInt, "Int", "put an integer into the bottle");
	REG_CMD(addFloat, "Float", "put a float into the bottle");
	REG_CMD(display, "Display", "display current bottle content");
	REG_CMD(send, "Send", "send current bottle");
	REG_CMD(reset, "Reset", "reset (empty) current bottle");
}

void _help()
{
	COMMAND_TABLE_IT it(_commands);
	cout << "--------- MENU ---------\n";
	cout << "- Program commands:\n";
	cout << "enter: this menu\n";
	cout << "q!: quit program\n";
	cout << "- Commands:\n";
	while(!it.done())
	{
		cout << (*it).command << ": " << (*it).help << "\n";
		it++;
	}
	cout << "------------------------\n";
}

bool _parse(const YARPString &c, YARPBabyBottle &b)
{
	COMMAND_TABLE_IT it(_commands);
	while(!it.done())
	{
		COMMAND_TABLE_ENTRY &tmp = (*it);

		if (tmp.command == c)
		{
			// command registered
			return tmp.f(b);
		}
		it++;
	}

	cout << "Sorry, " << c << " command not supported\n";
	return false;
}

int main(int argc, char* argv[])
{
	_fillTable();

	if (YARPParseParameters::parse(argc, argv, "interactive"))
		interactive(argc, argv);
	else
		command(argc, argv);

	return 0;
}


void interactive(int argc, char* argv[])
{
	cout << "You chose interactive mode\n";
	YARPString name;
	if (!YARPParseParameters::parse(argc, argv, "basename", name))
		_outPort.Register(YARPString(DEFAULT_NAME).append("/o").c_str());
	else
		_outPort.Register(name.append("/o").c_str());
	
	YARPBabyBottle tmp;
	YARPString c;
	bool quit = false;
		
	_help();
	while(cin>> c)
	{
		if (c=="")
			_help();
		else if (c=="q!")
			break;
		else
			_parse(c, tmp);
	}
}

void command(int argc, char* argv[])
{
	YARPString portName;
	YARPString remotePortName;

	portName = YARPString("/");
	portName.append(argv[0]);
	char tmp[80];
	srand( (unsigned)time( NULL ) );
	sprintf(tmp, "%d/o:bot", rand());
	portName.append(tmp);
	_outPort.Register(portName.c_str());
	
	if (!YARPParseParameters::parse(argc, argv, "port", remotePortName))
		error(PARAMETERS);

	YARPString notRemotePortName("!");
	notRemotePortName.append(remotePortName);

	// connect ports
	if (_outPort.Connect(portName.c_str(), remotePortName.c_str()) != YARP_OK)
	{
		// is port already connected ?? disconnect first
		_outPort.Connect(portName.c_str(), notRemotePortName.c_str());
		// try again
		if (_outPort.Connect(portName.c_str(), remotePortName.c_str()) != YARP_OK)
			error(CONNECT);	// ok something is wrong
	}
	
	// beginning building the bottle
	int index = 3;
	int n = argc-3;
	YARPBabyBottle bottle;

	while(n>1)	//a valid parameter consists in two strings
	{
		YARPString codeTxt("-text");
		YARPString codeFloat("-float");
		YARPString codeInt("-int");
		YARPString codeVocab("-vocab");
		YARPString codeLabel("-label");

		YARPString current(argv[index]);

		if (current == codeTxt)
		{
			// adding text
			bottle.writeText(argv[index+1]);
		}
		else if (current == codeFloat)
		{
			// add a float
			float tmp = atof(argv[index+1]);
			bottle.writeFloat(tmp);
		}
		else if (current == codeVocab)
		{
			// add vocab
			bottle.writeVocab(argv[index+1]);
		}
		else if (current == codeInt)
		{
			// add int
			int tmp = atoi(argv[index+1]);
			bottle.writeInt(tmp);
		}
		else if (current == codeLabel)
		{
			// adding text
			bottle.setID(argv[index+1]);
		}
		else
			error(PARSE_BOTTLE);
		
		index+=2;
		n-=2;
	}

	bottle.display();
	_outPort.Content() = bottle;
	Sleep(500);
	_outPort.Write(1);

	// disconnect
	_outPort.Connect(portName.c_str(), notRemotePortName.c_str());
}

void error(ERROR_CODE err)
{
	switch(err)
	{
	case PARSE_BOTTLE:
		ACE_OS::printf("Error parsing bottle check parameter list");
    break;
    case CONNECT:
		ACE_OS::printf("Error during connection to remote port, check its name\n");
		break;
	case PARAMETERS:
		ACE_OS::printf("Error parsing parameters, check list\n");
		break;
	case GENERIC:
	default:
		ACE_OS::printf("A generic error occurred\n");
	break;
	}

	printMenu();
    exit(-1);
}

void printMenu()
{
	ACE_OS::printf("-- Usage:\n");
	ACE_OS::printf("Mandatory: -port remote_port_name\n");
	ACE_OS::printf("Build your bottle:\n");
	ACE_OS::printf("-text mystring\n");
	ACE_OS::printf("-float 0.234\n");
	ACE_OS::printf("-int 1020\n");
	ACE_OS::printf("-vocab mystring\n");
	ACE_OS::printf("\n\n");
}