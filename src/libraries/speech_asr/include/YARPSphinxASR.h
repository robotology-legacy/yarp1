#ifndef YARPSphinxASR_INC
#define YARPSphinxASR_INC

#include <stdio.h>

class YARPSphinxASR
{
public:

  class Phrase
  {
  public:
    void *data;
    int valid;
    int word_count;

    Phrase() { valid = 0;  data = NULL;  word_count = 0; }

    int IsValid() { return valid; }

    int GetWordCount() { return word_count; }

    int GetDuration(int index);  // result in ms

    void GetText(int index, char *buffer, int buffer_length);

    void SaveWave(const char *filename);
  };

  virtual void OnHearingBegins() { printf("*** hearing begins\n"); }
  virtual void OnHearingEnds()   { printf("*** hearing ends\n");   }
  virtual void OnHearingSpeech() { printf("*** heard speech\n");   }

  virtual void OnHeardPhrase(Phrase& phrase) 
  {
    printf("*** recognized phrase\n");
  }

  void CreateVocabulary(const char *vocabulary);
  void AddPhrase(const char *phrase, const char *vocabulary);
  void RemovePhrase(const char *phrase, const char *vocabulary);

  void Main(int argc, char *argv[]);
};

#endif
