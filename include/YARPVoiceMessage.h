#ifndef YARPVoiceMessage_INC
#define YARPVoiceMessage_INC

#include "YARPAll.h"

#include <assert.h>

#include "YARPString.h"

//using namespace std;

#include "YARPNetworkTypes.h"
#include "YARPPortContent.h"

class YARPVoiceMessage
{
public:
  struct 
  {
    NetInt32 flags;
  } settings;

  string text;

  string& GetText() { return text; }
};

class YARPVoicePortContent : public YARPPortContent, public YARPVoiceMessage
{
public:
  NetInt32 len;

  virtual int Read(YARPPortReader& reader)
    { 
      reader.Read((char*)(&settings),sizeof(settings));
      reader.Read((char*)(&len),sizeof(len));
      /*
      if (text.max_size()<len-1)
	{
	  text.reserve(len);
	}
      return reader.Read((char*)text.c_str(),len);
      */
      //printf("Length is %d\n", len);
      assert(len<256);
      char buf[256] = "";
      int result = reader.Read((char*)(&buf[0]),len);
      text = buf;
      return result;
    }
  
  virtual int Write(YARPPortWriter& writer)
    { 
      len = text.length()+1;
      writer.Write((char*)(&settings),sizeof(settings));
      writer.Write((char*)(&len),sizeof(len));
      return writer.Write((char*)(text.c_str()),len); 
    }

  virtual int Recycle()
    { return 0; }

};

class YARPInputPortOf<YARPVoiceMessage> : public YARPBasicInputPort<YARPVoicePortContent>
{
};

class YARPOutputPortOf<YARPVoiceMessage> : public YARPBasicOutputPort<YARPVoicePortContent>
{
};


#endif

