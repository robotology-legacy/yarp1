/////////////////////////////////////////////////////////////////////////
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
///
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
/// $Id: Sendable.cpp,v 1.3 2003-06-28 16:40:01 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/OS.h>

#include "Sendable.h"
#include "Sendables.h"
#include "RefCounted.h"


int Sendable::Destroy()
{
	int destroyed = 0;
	if (owner!=NULL)
	{
		owner->TakeBack(this);
		destroyed = 0;
	}
	else
	{
		ACE_OS::printf(">>> Sendable::Destroy() killed 0x%x\n", ((int) (this)));
		delete this;
		destroyed = 1;
	}
	return destroyed;
}



/*
template <class T>
class SimplePortable
{
public:
  T datum;

  SendablesOf<SimpleSendable<T> > sendables;

  void Fire(Port& port)
    {
      SimpleSendable<T> *sendable = sendables.Get();
      if (sendable == NULL) sendable = new SimpleSendable<T>;
      assert(sendable!=NULL);
      sendable->owner = &sendables;
      sendable->datum = datum;
      //port.Give(sendable);
    }

  void PermitModification()
    {
    }

  void PermitOverwrite()
    {
    }
};

SimplePortable<int> x;
*/

/*
class ImageSendable : public Sendable
{
public:
  struct Header
  {
    int len;
    int w;
    int h;
    int depth;
  } header;
  char *buffer;
  int is_owner;

  ImageSendable()
    {
      buffer = NULL;
      is_owner = 0;
    }

  virtual ~ImageSendable()
    {
      printf("Imagesendable DESTRUCTOR\n");
      if (is_owner&&buffer!=NULL)
	{
	  delete[] buffer;
	  buffer = NULL;
	}
    }

  virtual int Fire(BlockSender& sender)
    {
      assert(buffer!=NULL);
      sender.Add((char*)(&header),sizeof(header));
      sender.Add(buffer,header.len);
      return 1;
    }
};
*/

/*
class ImagePortable : public GenericImage
{
public:

  SendablesOf<ImageSendable > sendables;
  ImageSendable *last_sendable;

  ImagePortable() 
    { last_sendable = NULL; }

  virtual ~ImagePortable()
    { if (last_sendable!=NULL) last_sendable->RemoveRef(); }

  void Fire(Port& port)
    {
      ImageSendable *sendable = sendables.Get();
      if (sendable == NULL) sendable = new ImageSendable;
      // set target to sendables HIT
      assert(sendable!=NULL);
      sendable->owner = &sendables;
      sendable->header.h = GetHeight();
      sendable->header.w = GetWidth();
      //port.Give(sendable);
    }

  void PermitModification()
    {
    }

  void PermitOverwrite()
    {
    }
};

ImagePortable y;

*/


/*
template <class T>
class IFragment
{
public:
  void SwitchFragments(T& resource);
  void CopyFragments(T& resource);
};

class Sendable
{
public:
  virtual void Fire(BlockSender& sender) {}
  virtual ~SendablePackage() {}
  virtual void Release() {}
};

template <class T>
class SendablePackageOf
{
public:
  T datum;
  virtual void Send(BlockSender& sender)
    {
      sender.Add((char*)(&datum),sizeof(datum));
    }
};

class SendablePackageOfImage
{
public:
  struct Header
  {
    int w;
    int h;
  } header;
  virtual void Send(BlockSender& sender)
    {
      sender.Add((char*)(&header),sizeof(header));
      
    }
};

class Sendable
{
public:
  Sema sender_count;

  Sendable() : sender_count(0)
    {}
};
*/

/*
void main()
{
}
*/
