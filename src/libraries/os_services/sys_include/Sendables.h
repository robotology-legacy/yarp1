
#ifndef SENDABLES_H_INC
#define SENDABLES_H_INC

//#include "debug-new.h"

#include <list>
#ifndef __QNX__
using namespace std;
#endif

#include "Sendable.h"

//#include <new.h>
//#include <defalloc.h>

//#include "debug-new.h"

#include "YARPSemaphore.h"

extern YARPSemaphore refcounted_sema;


class PSendable
{
public:
  Sendable *sendable;
  PSendable(Sendable *nsendable=NULL) { sendable = nsendable; }

  Sendable *Content() { return sendable; }

  ~PSendable()
    {
      if (sendable!=NULL)
	{
//	  printf("!!!!!!!!!!!! PSendable DESTRUCTOR! %ld\n", (long int) sendable);
	  delete sendable;
	}
      sendable = NULL;
    }
  
  int operator == (const PSendable& s) const
    { return 1; }
  int operator != (const PSendable& s) const
    { return 0; }
  int operator < (const PSendable& s) const
    { return 0; }
};

class Sendables
{
public:
  list<PSendable> sendables;

  void PutSendable(Sendable *s)
    {
      refcounted_sema.Wait();
      list<PSendable>::iterator cursor;
      //cout << "*** NEW stl " << __FILE__ << ":" << __LINE__ << endl;
      //sendables.insert(sendables.begin(),PSendable());
      //printf("PutSendable() > 1\n");
      sendables.push_back(PSendable());
      assert(s!=NULL);
      //printf("PutSendable() > 2\n");
      //      sendables.insert(sendables.begin(),*s);
      assert(sendables.begin()!=sendables.end());
      //printf("PutSendable() > 3\n");
      cursor = sendables.end();
      --cursor;
      //printf("PutSendable() > 4\n");
      (*cursor).sendable = s;
      s->owner = this;
      //printf("PutSendable() > 1\n");
      refcounted_sema.Post();
    }

  void TakeBack(Sendable *s)
    {
      //printf("##### Take back called for %ld (%d)\n", (long int) s, s->ref_count);
      PutSendable(s);
      //printf("##### POST Take back called for %ld (%d)\n", (long int) s, s->ref_count);
    }
  
  Sendable *GetSendable()
    {
      Sendable *s = NULL;
      refcounted_sema.Wait();
      list<PSendable>::iterator cursor = sendables.end();
      if (sendables.begin()!=sendables.end())
	{
	  --cursor;
	  s = (*cursor).sendable;
	  //printf("### %d for %d\n", s->ref_count, (long int)s);
	  (*cursor).sendable = NULL;
	  sendables.pop_back();	
	  assert(s!=NULL);
	  //printf("### %d for %d\n", s->ref_count, (long int)s);
	  s->ref_count = 0;
	}
      refcounted_sema.Post();
      return s;
    }
};

template <class T>
class SendablesOf : public Sendables
{
public:
  void Put(T *s)
    {
      PutSendable(s);
    }

  T *Get()
    {
      T *t = (T*)GetSendable();
      if (t==NULL)
	{
	  //T *buf = (T *)new char[sizeof(T)];
	  //assert(buf!=NULL);
	  //t = new (buf) T;
	  t = new T;	
	  assert(t!=NULL);
	  t->ZeroRef();
	}
      assert(t!=NULL);
      t->owner = this;
      return t;
    }
};


#endif
