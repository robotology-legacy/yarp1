/*
  This is not yet fully implemented
 */


#ifndef YARPPhoneme_INC
#define YARPPhoneme_INC

#include <list>
using namespace std;

enum
{
  PHONEMES_NULL,
  PHONEMES_VIAVOICE_ASR,
  PHONEMES_VIAVOICE_TTS,
  PHONEMES_SLS,
};

class YARPPhoneme
{
public:
  int mode;
  long int key;

  void Render(int phoneme_class, char *buffer, int buffer_length);
};

typedef list<YARPPhoneme> YARPPhonemes;

class YARPPhonemeConverter
{
public:
  YARPPhoneme ConvertSingle(int phoneme_class, const char *text);
  YARPPhonemes ConvertMultiple(int phoneme_class, const char *text);
};

#endif

