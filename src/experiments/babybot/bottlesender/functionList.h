

#include <yarp/YARPBabyBottle.h>

typedef bool (*myFunction)(YARPBabyBottle &bottle);

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
bool addYVector(YARPBabyBottle &bottle);
bool addYVocab(YARPBabyBottle &bottle);
bool addText(YARPBabyBottle &bottle);
bool addInt(YARPBabyBottle &bottle);
bool addFloat(YARPBabyBottle &bottle);
bool display(YARPBabyBottle &bottle);
bool reset(YARPBabyBottle &bottle);
bool send(YARPBabyBottle &bottle);
bool addID(YARPBabyBottle &bottle);
