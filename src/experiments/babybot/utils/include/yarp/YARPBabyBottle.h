/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: YARPBabyBottle.h,v 1.2 2004-08-11 13:29:20 babybot Exp $
///
///

#ifndef __YARPBabyBottleh__
#define __YARPBabyBottleh__

#include <yarp/YARPBottle.h>
#include <yarp/YARPMatrix.h>

#include <stdio.h>

class YARPBabyBottle : public YARPBottle {
public:

  void writeYVector(const YVector &in)
  {
    writeRawInt(YBTypeYVector);
    writeRawInt(in.Length());
    writeRawBlock((char *) in.data(), sizeof(double)*in.Length() );
  }
   

  bool tryReadYVector(YVector &v)
  {
    int oldIndex = index;
    lastReadSeq = 0;
    if (!assertType(YBTypeYVector))
			return false;
    index+=sizeof(YBTypeYVector);
    int l = readRawInt();
    index+=sizeof(int);
    v.Resize(l);
    memcpy(v.data(), readRawBlock(l*sizeof(double)), l*sizeof(double)); 
    index = oldIndex;
    return true;
  }


  bool readYVector(YVector &v)
  {
	  if (tryReadYVector(v))
	  {
		moveOn();
		return true;
	  }
	  else
		return false;
  }


  void display()
  {
    ACE_OS::printf("%s: ", id.c_str());
    rewind();
    int first = 1;
    while (more())
      {
	char *str;
	int ch = readRawInt();
	_moveOn(sizeof(enum __YBTypeCodes));
	if (!first)
	  {
	    printf(" ");
	  }
	first = 0;
	  switch(ch)
	    {
	    case YBTypeInt:
	      printf("%d", readRawInt());
	      _moveOn(sizeof(int));
	      break;
	    case YBTypeVocab:
	      str = (char *) readRawText();
	      printf("<%s>", str);
	      _moveOn(strlen(str)+1+sizeof(int));
	      break;
	    case YBTypeDouble:
	      printf("%g", readRawFloat());
	      _moveOn(sizeof(double));
	      break;
	    case YBTypeString:
	      str = (char *) readRawText();
	      printf("(%s)", str);
	      _moveOn(strlen(str)+1+sizeof(int));
	      break;
	    case YBTypeYVector:
	      {
		index = index-sizeof(int);
		YVector tmp;
		tryReadYVector(tmp);
		int i;
		printf("<");
		for(i = 1; i < tmp.Length(); i++)
		  {
		    printf("%g ", tmp(i));
		  }
		printf("%g>", tmp(i)); // last item
		moveOn();
	      }
	      break;
	    default:
	      printf("???");
	      break;
	    }
      }
    printf("\n");
    rewind();
  }
};


/*
  input and output ports should hopefully just work... maybe not :)
 */

class YARPBabyBottleContent : public YARPBabyBottle, public YARPPortContent
{
public:
	virtual ~YARPBabyBottleContent () 
	{
		///ACE_DEBUG ((LM_DEBUG, "destroying a YARPBottleContent\n"));
	}

	virtual int Read(YARPPortReader& reader)
    {
	  // read id
	  reader.Read((char*)(&id.length), sizeof(id.length));
	  reader.Read(id.text, id.length);
	  // read id
	  reader.Read((char*)(&len),sizeof(len));

	  text.reserve(len);
      text.resize(len);

	  top = len;
      int result = reader.Read((char*)(&text[0]),len);
	  rewind();
      return result;
    }
  
  virtual int Write(YARPPortWriter& writer)
    {
	  writer.Write((char*)(&id.length),sizeof(id.length));
	  writer.Write((char*) id.text, id.length);
	  len = top; 
      
      writer.Write((char*)(&len),sizeof(len));
      int r = writer.Write((char*)(&text[0]),len);
      return r;
    }
  
  virtual int Recycle()
    { reset(); top = 0; return 0; }

    YARPBabyBottle& operator=(const YARPBabyBottle &vec) { return YARPBabyBottle::operator= (vec); }
};


class YARPInputPortOf<YARPBabyBottle> : public YARPBasicInputPort<YARPBabyBottleContent>
{
public:
	YARPInputPortOf<YARPBabyBottle>(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicInputPort<YARPBabyBottleContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPInputPortOf<YARPBabyBottle> () {  YARPPort::End(); }
};

class YARPOutputPortOf<YARPBabyBottle> : public YARPBasicOutputPort<YARPBabyBottleContent>
{
public:
	YARPOutputPortOf<YARPBabyBottle>(int n_service_type = DEFAULT_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicOutputPort<YARPBabyBottleContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPOutputPortOf<YARPBabyBottle> () {  YARPPort::End(); }
};

#endif