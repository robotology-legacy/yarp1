#include <YARPBottle.h>

typedef bool (*myFunction)(YARPBottle &bottle);

struct COMMAND_TABLE_ENTRY
{
	YARPString command;
	YARPString help;
	myFunction f;
};

typedef YARPList<COMMAND_TABLE_ENTRY> COMMAND_TABLE;
typedef COMMAND_TABLE::iterator COMMAND_TABLE_IT;

// register command macro, for now verbose is equal to VOCABENTRY
#define REG_CMD(MYFUNCTION, VERBOSE, HELPSTRING) { COMMAND_TABLE_ENTRY tmp;\
													tmp.command = YARPString(VERBOSE);\
													tmp.help = YARPString(HELPSTRING);\
													tmp.f = MYFUNCTION;\
													_commands.push_back(tmp);}

// function list
bool addYVector(YARPBottle &bottle);
bool addYVocab(YARPBottle &bottle);
bool addText(YARPBottle &bottle);
bool addInt(YARPBottle &bottle);
bool addFloat(YARPBottle &bottle);
bool display(YARPBottle &bottle);
bool reset(YARPBottle &bottle);
bool send(YARPBottle &bottle);
