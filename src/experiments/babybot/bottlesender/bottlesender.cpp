// bottlesender.cpp : Defines the entry point for the console application.
//

#include <YARPPort.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPParseParameters.h>
#include <./conf/YARPVocab.h>
#include "functionList.h"

const char *DEFAULT_NAME = "/bottlesender";

/*static */ YARPOutputPortOf<YARPBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
COMMAND_TABLE _commands;

void _fillTable()
{
	// global commands
	REG_CMD(addYVector, "YVector", "put an YVector into the bottle");
	REG_CMD(addYVocab, "YBVocab", "put an YBVocab into the bottle");
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

bool _parse(const YARPString &c, YARPBottle &b)
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

	YARPString name;
	if (!YARPParseParameters::parse(argc, argv, "name", name))
		_outPort.Register(YARPString(DEFAULT_NAME).append("/o").c_str());
	else
		_outPort.Register(name.append("/o").c_str());
	
	YARPBottle tmp;
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

	return 0;
}
