#include <YARPBottle.h>

typedef bool (*myFunction)(const YBVocab &vocab, YARPBottle &bottle);

struct COMMAND_TABLE_ENTRY
{
	YARPString verbose;
	YARPString help;
	YBVocab	   command;
	myFunction f;
};

typedef YARPList<COMMAND_TABLE_ENTRY> COMMAND_TABLE;
typedef COMMAND_TABLE::iterator COMMAND_TABLE_IT;

// register command macro, for now verbose is equal to VOCABENTRY
#define REG_VOCAB(MYFUNCTION, VOCABENTRY, HELPSTRING) { COMMAND_TABLE_ENTRY tmp;\
														tmp.verbose = YARPString(VOCABENTRY);\
														tmp.command = YBVocab(VOCABENTRY);\
														tmp.help = YARPString(HELPSTRING);\
														tmp.f = MYFUNCTION;\
														_commands.push_back(tmp);}

bool sendSimpleVocab(const YBVocab &vocab, YARPBottle &bottle);
bool sendArmJointCommand(const YBVocab &vocab, YARPBottle &bottle);
bool sendHeadJointCommand(const YBVocab &vocab, YARPBottle &bottle);
bool sendHandJointCommand(const YBVocab &vocab, YARPBottle &bottle);

