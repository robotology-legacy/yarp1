///////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPBottle.h,v 1.4 2004-07-27 16:51:43 babybot Exp $
///
///
/// This code is based on the old YARPBottle class.
/// Note, this class has the same name but acts differently. The new YARPBottle
/// merges together the YARPBottle and the YARPBottleIterator classes.
/// To read messages within the bottle use the "tryRead" functions. Subsequent calls 
/// to tryRead return the same value, unless moveOn() is called.
/// moveOn() has effect only once after each tryRead and move the internal index right
/// after the data that were read by the last tryRead (if successful).
///
/// July 2003 --by nat 

#ifndef YARPBOTTLE_INC
#define YARPBOTTLE_INC

#include <assert.h>
#include <stdio.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPPortContent.h>
#include <yarp/YARPBottleCodes.h>
//#include <yarp/YARPVector.h>
#include <yarp/YARPString.h>
#include <yarp/YARPList.h>
#include <yarp/YARPNetworkTypes.h>

class YARPBottle
{
	struct BottleId
	{
		BottleId()
		{
		//	text.resize(__maxBottleID);
		}

		void set(const char *s)
		{ 
			strncpy(text, s , __maxBottleID-1);
			text[__maxBottleID-1] = '\0';	// paranoid ?
			length = strlen(text)+1;
		}

		void set(const YBVocab &s)
		{
			set(s.c_str());
		}

		bool operator !=(const BottleId &l) const
		{
			return !operator==(l);
		}
		
		bool operator==(const BottleId &l) const
		{
			if (strcmp(text, l.c_str()) == 0)
				return true;
			else
				return false;
		}

		bool operator !=(const YBVocab &l) const
		{
			return !operator==(l);
		}
		
		bool operator==(const YBVocab &l) const
		{
			if (strcmp(text, l.c_str()) == 0)
				return true;
			else
				return false;
		}

		const char *c_str() const
		{
			return text;
		}

		char text[__maxBottleID];
		NetInt32 length;
	};

public:
	YARPBottle();

	YARPBottle& operator = (const YARPBottle& bottle)
	{
	  id.set(bottle.id.text);
      top = bottle.top;
	  text = bottle.text;
	  lastReadSeq = bottle.lastReadSeq;
	  index = bottle.index;
	  return *this;
	}
   
	YARPVector<char>& getBuffer() { return text; }

	void setID(const YBVocab &l) { id.set(l.c_str()); }
	const BottleId &getID() const { return id; }
	int getSize() const { return text.size(); }
	int getTopInBytes() const { return top; }
	const char *getDataPtr() const { return &text[0]; } 
	void setDataPtr(char *buf, int l) { writeRawBlock(buf, l); }
  
	// consecutive writes add data in the buffer
	void writeInt(NetInt32 result)
		{ writeRawInt(YBTypeInt);  writeRawInt(result); }
	void writeFloat(double result)
		{ writeRawInt(YBTypeDouble);  writeRawFloat(result); }
	void writeText(const char *result)
		{ writeRawInt(YBTypeString);  writeRawText(result); }

	void writeVocab(const YBVocab &result)
	{
		writeRawInt(YBTypeVocab);
		writeRawText(result.c_str());
	}

	/*
	void writeYVector(const YVector &in)
	{
		writeRawInt(YBTypeYVector);
		writeRawInt(in.Length());
		writeRawBlock((char *) in.data(), sizeof(double)*in.Length() );
	}
	*/
  
	// "try" functions; The rationale is: return true if the type of the next 
	// variable in the bottle matches the one of the function you call, 
	// otherwise return false. The output parameter in this case is undefined.
	// The internal "index" is not changed, that is the next "tryRead" will occur
	// on the same data, unless the moveOn() function is called.
	bool tryReadInt(int *i)
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeInt))
			return false;
		index+=sizeof(YBTypeInt);
		*i = (int) readRawInt();
		index = oldIndex;
		return true;
	}

	/*
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
	*/

	bool tryReadFloat(double *f)
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeDouble))
			return false;
		index+=sizeof(YBTypeDouble);
		*f = readRawFloat();
		index = oldIndex;
		return true;
	}

	bool tryReadText(char *s)
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeString))
			return false;
		index+=sizeof(YBTypeString);
		readRawText(s);
		index = oldIndex;
		return true;
	}

	// nice to be able to read text without allocating external
	// buffer, if you own the bottle
	const char *tryReadText()
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeString))
			return NULL;
		index+=sizeof(YBTypeString);
		const char *addr = readRawText();
		index = oldIndex;
		assert(addr!=NULL);
		return addr;
	}

	bool tryReadVocab(YBVocab &v)
	{
		int oldIndex = index;
		lastReadSeq = 0;
		if (!assertType(YBTypeVocab))
			return false;
		index+=sizeof(YBTypeVocab);
		v = YBVocab(readRawText());
    	index = oldIndex;
		return true;
	}

	/*
	void readYVector(YVector &v)
	{
		if (tryReadYVector(v))
			moveOn();
	}
	*/
  
	void readInt(int *v)
	{
		if (tryReadInt(v))
			moveOn();
	}

	int readInt() {
	  int v = 0;
	  readInt(&v);
	  return v;
	}

	void readFloat(double *v)
	{
		if (tryReadFloat(v))
			moveOn();
	}
	
	// yikes! no way to bound size of buffer -
	// invitation to disaster.
	void readText(char *s)
	{
		if (tryReadText(s))
			moveOn();
	}

	const char *readText()
	{
	  const char *addr = tryReadText();
	  if (addr!=NULL) {
	    moveOn();
	  }
	  return addr;
	}

	void rewind()
	{ index = 0;}

	void reset()
	{
		rewind();
		top = 0;
	}
  
	int more() const
	{
	  return (index<top-1);
	}

	// moveOn(). Increase "index". The next "tryRead" will be on the remaining
	// part of the buffer. Subsequent calls to moveOn() do nothing unless another
	// tryRead is called.
	void moveOn()
	{
		_moveOn(lastReadSeq);
		lastReadSeq = 0;
	}

	void _moveOn(int l)
	{
		index += l;
		if (index >top)
			index = top;
	}
	
	void dump();
	void display();

protected:
	char *readRawBlock(int len)
	{
		assert(getSize()-index>=len);
		char *result = (&(text)[index]);
		lastReadSeq += len;
		return result;
    }

	void readRawBlock(char *buf, int len)
    {
		char *src = readRawBlock(len);
		memcpy(buf,src,len);
    }
 
	void writeRawBlock(char *buf, int l)
    {
		text.resize(index+l);
		memcpy(&text[index],buf,l);
		index+=l;
		top = index;
		len = top;
    }
  
	char readRawChar()
    {
		char result;  
		readRawBlock((char*)(&result),sizeof(result));
		return result;
    }
	
	void writeRawChar(char result)
	{
		writeRawBlock((char*)(&result),sizeof(result));
	}

	NetInt32 readRawInt()
    {
		NetInt32 result;  
		readRawBlock((char*)(&result),sizeof(result));
		return result;
    }

	void writeRawInt(NetInt32 result)
    {
		writeRawBlock((char*)(&result),sizeof(result));
    }

	double readRawFloat()
    {
		double result;  
		readRawBlock((char*)(&result),sizeof(result));
		return result;
    }

	void writeRawFloat(double result)
    {
		writeRawBlock((char*)(&result),sizeof(result));
    }
  
	const char *readRawText()
    {
		NetInt32 len = readRawInt();
		index += sizeof(NetInt32);
		const char *tmp = readRawBlock(len);
		index -= sizeof(NetInt32);
		return tmp;
    }
	
	void readRawText(char *s)
    {
      //printf("*** please supply a buffer size to YARPBottle::readRawText and related fns\n");
		NetInt32 len = readRawInt();
		index += sizeof(int);
		memcpy(s, readRawBlock(len), len);
		index -= sizeof(int);
    }

	void writeRawText(const char *text)
    {
		writeRawInt(strlen(text)+1);
		writeRawBlock((char*)(&text[0]),strlen(text)+1);      
    }
	
	bool assertType(int ch)
    {
		// first of all check if bottle is !empty
		if (!more())
			return false;
		int compare;
		compare = readRawInt();
		return (ch==compare);
    }

	YARPVector<char> text;
	BottleId id;
	NetInt32 len;
	NetInt32 top;
	int index;
	// this is used to "remove" the last read sequence from bottle
	int lastReadSeq;
};

// circular dependency for now - don't want end user to forget
// about the Content.  This can be a very hard bug to track down.
#include <yarp/YARPBottleContent.h>

#endif

