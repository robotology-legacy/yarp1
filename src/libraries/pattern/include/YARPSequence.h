#ifndef YARPSEQUENCE_INC
#define YARPSEQUENCE_INC

#include <stdlib.h>
#include <string>
#include <list>
#ifndef __QNX__
#include <hash_map>
#else
#include <map>
#endif
#include <iostream>

typedef unsigned int YARPToken;


struct YARPAutomatonEntry
{
  YARPToken id;
  YARPToken emit;
  list<YARPToken> targets;
};


class YARPAutomaton : 
  public 
#ifndef __QNX__
std::
#endif
           hash_map<YARPToken, YARPAutomatonEntry, hash<YARPToken>, 
                       equal_to<YARPToken> >
{
public:
  friend ostream& operator<<(ostream& os, const YARPAutomaton& automaton);
  
  void Show(ostream& os) const;
};

ostream& operator<<(istream& os, const YARPAutomaton& automaton);


class YARPAutomatonMaker
{
protected:
  void *resources;
  void StartResources();
  void StopResources();
public:
  YARPAutomatonMaker() { resources=NULL; StartResources(); }
  virtual ~YARPAutomatonMaker() { StopResources(); }

  // user interface
  void Clear();
  void Add(const YARPToken& t);
  void Add(const char *str);
  bool Iterate();
  void Complete();

  void GetAutomaton(YARPAutomaton& automaton);

  void GetHistogram();
};


class YARPDictionary
{
public:
  static std::string GetText(const YARPToken& loc);

  static YARPToken GetToken(const char *str);

  static std::string GetText(std::istream& is, int *ok = NULL);

  static YARPToken GetToken(std::istream& is, int *ok = NULL)
    {
      int my_ok = 0;
      YARPToken yt = 0;
      string s = GetText(is,&my_ok);
      if (ok!=NULL) *ok = my_ok;
      if (ok) yt = GetToken(s.c_str());
      return yt;
    }
};

class YARPSequence
{
public:
  list<YARPToken> seq;

  void Add(const YARPToken& yt)
    { seq.push_back(yt); }

  void Add(const char *str)
    { Add(YARPDictionary::GetToken(str)); }

  void Parse(YARPAutomaton& automaton, YARPSequence& target);

  friend ostream& operator<<(ostream& os, const YARPSequence& ys);
};

ostream& operator<<(ostream& os, const YARPSequence& ys);

class YARPTimedSequence : public YARPSequence
{
public:
  list<double> seq_time;

  void Add(const YARPToken& yt, double t = -1)
    { seq.push_back(yt);  seq_time.push_back(t); }

  void Add(const char *str, double t = -1)
    { Add(YARPDictionary::GetToken(str),t); }

  void Parse(YARPAutomaton& automaton, YARPTimedSequence& target)
    {
      YARPSequence::Parse(automaton,target);
      target.seq_time = seq_time;
    }
};


#endif

