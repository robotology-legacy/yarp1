
#include <yarp/YARPBabyBottle.h>

typedef bool (*myFunction)(const YBVocab &vocab, YARPBabyBottle &bottle);

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

bool sendSimpleVocab(const YBVocab &vocab, YARPBabyBottle &bottle);
bool sendArmJointCommand(const YBVocab &vocab, YARPBabyBottle &bottle);
bool sendHeadJointCommand(const YBVocab &vocab, YARPBabyBottle &bottle);
bool sendHandJointCommand(const YBVocab &vocab, YARPBabyBottle &bottle);
bool sendHandKinematicsView(const YBVocab &vocab, YARPBabyBottle &bottle);
bool sendString(const YBVocab &vocab, YARPBabyBottle &);

