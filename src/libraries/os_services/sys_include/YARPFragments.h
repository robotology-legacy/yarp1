#ifndef FRAGMENTS_INC
#define FRAGMENTS_INC

#include <assert.h>

class Fragments
{
protected:
  Fragments *next;
public:
  char *buffer;
  int len;
  int clen;
  int owned;

  Fragments()
    { buffer = NULL;  clen = len = 0;  owned = 0;  next = NULL;}

  Fragments(char *nbuffer, int nlen, int nowned)
    { buffer = nbuffer;  clen = len = nlen;  owned = nowned;  next = NULL; }

  virtual ~Fragments()
    { ClearChildren(); Clear(); }

  void Clear()
    { 
      if (owned&&buffer!=NULL) delete[] buffer;
      buffer = NULL;  clen = len = 0;  owned = 0; 
    }

  void Require(int nlen)
    {
      if (nlen<1) nlen = 1;
      if (buffer!=NULL && nlen<=len)
	{
	  clen = nlen;
	}
      else
	{
	  Clear();
	  buffer = new char[nlen];
	  assert(buffer!=NULL);
	  len = nlen;
	  clen = nlen;
	  owned = 1;
	}
    }

    //accident with search-and-replace
  void Take(Fragments& fragments)
    {
      buffer = fragments.buffer;
      len = fragments.len;
      clen = fragments.clen;
      owned = fragments.owned;
      fragments.buffer = NULL;
      fragments.Clear();
    }

  void SwitchOne(Fragments& fragments)
    {
      int minlen = fragments.len;
      Fragments tmp;
      tmp.Take(fragments);
      fragments.Take(*this);
      fragments.Require(minlen);
      Take(tmp);
    }

  void Switch(Fragments& fragments)
    {
      SwitchOne(fragments);
      Fragments *tmp = next;
      next = fragments.next;
      fragments.next = tmp;
    }

  char *GetBuffer() { return buffer; }
  int GetLength() { return clen; }
  int GetAllocatedLength() { return len; }
  int IsOwner() { return owned; }

  Fragments *GetNext()
    { return next; }

  Fragments *AddNext()
    { if (next==NULL) next = new Fragments;  assert(next!=NULL); 
      return next;
    }

  void ClearChildren()
    {
      if (next!=NULL)
	{
	  delete next;
	  next = NULL;
	}
    }
};



template <class T>
void CopyToFragments(Fragments& Fragments, const T& t)
{
  Fragments.Require(sizeof(t));
  memcpy(Fragments.GetBuffer(),(char *)(&t),sizeof(t));
}

template <class T>
void CopyFromFragments(Fragments& Fragments, const T& t)
{
  assert(Fragments.GetLength()==sizeof(t));
  memcpy((char *)(&t),Fragments.GetBuffer(),sizeof(t));
}


#endif
