/*
  This is unfinished
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "YARPPhoneme.h"

YARPPhoneme YARPPhonemeConverter::ConvertSingle(int phoneme_class, 
						const char *text)
{
  YARPPhoneme result;
  assert(text!=NULL);
  result.mode = phoneme_class;
  int ch1 = '\0';
  int ch2 = '\0';
  int ch3 = '\0';
  if (*text!='\0')
    {
      ch1 = *text;
      text++;
      if (*text!='\0')
	{
	  ch2 = *text;
	  text++;
	  if (*text!='\0')
	    {
	      ch3 = *text;
	    }
	}
    }
  result.key = ch1*65536+ch2*256+ch3;
  return result;
}

YARPPhonemes YARPPhonemeConverter::ConvertMultiple(int phoneme_class, 
						   const char *text)
{
  //YARPPhonemes result;
}


void YARPPhoneme::Render(int phoneme_class, char *buffer, int buffer_length)
{
  unsigned char ch[4] = "???";
  long int k = key;
  ch[2] = (unsigned char)(key%256);
  key /= 256;
  ch[1] = (unsigned char)(key%256);
  key /= 256;
  ch[0] = (unsigned char)(key);
  assert(buffer_length>=sizeof(ch));
  strncpy(buffer,(char*)
ch,sizeof(ch));
}

