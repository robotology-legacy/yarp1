#ifndef __YARPBOTTLECODESINC__
#define __YARPBOTTLECODESINC__

///
///
/// bottle types
enum __YBTypeCodes 
{
	YBTypeChar,
	YBTypeInt,
	YBTypeDouble,
	YBTypeYVector,
	YBTypeVocab,
	YBTypeString,
};


#ifdef __cplusplus

#include <YARPString.h>

const char * const YBLabelNULL	= "Null";
const char * const YBVIsAlive	= "IsAlive";
const int __maxBottleID = 80;

class YBVocab : public ACE_String_Base<char>
{
public:
	YBVocab () : ACE_String_Base<char> () {}
	YBVocab (const char *s) : ACE_String_Base<char> (s) {}
	YBVocab (const char *s, size_t len) : ACE_String_Base<char> (s, len) {}
	YBVocab (const ACE_String_Base<char>& s) : ACE_String_Base<char> (s) {}
	YBVocab (const YBVocab& s) : ACE_String_Base<char> (s) {}
	YBVocab (char c) : ACE_String_Base<char> (c) {}
	YBVocab (size_t len, char c = 0) : ACE_String_Base<char> (len, c) {}

	~YBVocab () { clear(1); }

	YBVocab& operator= (const YBVocab& s) { ACE_String_Base<char>::operator= (s); return *this; }
	
	YBVocab& append(const char *s) { ACE_String_Base<char>::operator+=(s); return *this; }
	YBVocab& append(const YBVocab& s) { ACE_String_Base<char>::operator+=(s); return *this; }
	bool empty (void) { return (length() == 0) ? true : false; }
};

#endif

#endif
