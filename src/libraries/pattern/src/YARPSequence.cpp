#include <assert.h>
#include <stdio.h>

#ifndef __QNX__
#include <fstream>
#include <hash_map>
#include <list>
#include <string>
using namespace std;
#else
#include <fstream.h>
#include <map.h>
#include <list.h>
#include <string>
#endif

#include "YARPRandomNumber.h"
#include "YARPSequence.h"

#include "YARPViterbi.h"

#define MAX_TOKEN 100000
#define MAX_DICT 1000
//#define MAX_CONTEXT 11
#define MAX_CONTEXT 2
#define MAX_NEXT 100

typedef long int Token;

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

struct ExternalToken
{
public:
  string s;
  string Get() { return s; }
  void Set(const char *n_s) { s = n_s; }
};

struct equal_external_token
{
  bool operator()(const ExternalToken& s1, const ExternalToken& s2) const
    {
      return s1.s == s2.s;
    }
};

struct hash_external_token
{
  size_t operator()(const ExternalToken& et) const
    {
      hash<const char *> h;
      return h(et.s.c_str());
    }
};


/*
bool YARPStringUnit::operator==(const YARPStringUnit& su)
{
  return s == su.s;
}


size_t YARPStringUnit::hash()
{
      hash<const char *> h;
      return h(s.c_str());
}
*/


struct State
{
  Token current;
  Token next[MAX_NEXT];
  int count;

  State() { count = 0;  current = 0; }

  void Set(Token t)
    { current = t; }

  void Add(Token t)
    { assert(count<MAX_NEXT);  next[count] = t;  count++; }

  Token Get()
    { return current; }

  int GetCount()
    {
      return count;
    }

  Token Get(int index)
    {
      assert(index>=0 && index<count);
      return next[index];
    }
};


struct hash_state
{
  size_t operator()(const State& state) const
    {
      size_t v = state.current;
      for (int i=0; i<state.count; i++)
	{
	  v *= 17 + 5;
	  v += state.next[i];
	}
      return v;
    }
};

struct equal_state
{
  bool operator()(const State& s1, const State& s2) const
    {
      bool v = (s1.current == s2.current);
      if (v)
	{
	  if (s1.count == s2.count)
	    {
	      for (int i=0; i<s1.count; i++)
		{
		  if (s1.next[i] != s2.next[i])
		    {
		      v = false;
		      break;
		    }
		}
	    }
	  else
	    {
	      v = false;
	    }
	}
      return v;
    }
};



typedef hash_map<int, int, hash<int>, equal_to<int> > hash_ii;
typedef hash_map<int, double, hash<int>, equal_to<int> > hash_id;
//typedef hash_map<const char*, int, hash<const char*>, eqstr> hash_si;
//typedef hash_map<int, const char*, hash<int>, equal_to<int> > hash_si;
typedef hash_map<int, State, hash<int>, equal_to<int> > hash_is;
typedef hash_map<State, int, hash_state, equal_state > hash_si;
typedef hash_map<ExternalToken, int, hash_external_token, 
  equal_external_token > hash_ei;
typedef hash_map<int, ExternalToken, hash<int>, equal_to<int> > hash_ie;



class Dictionary
{
public:
  hash_is dictionary;
  hash_ii numeric;
  hash_si stateful;
  hash_ie int_ext;
  hash_ei ext_int;
  hash_id identity_strength;
  Token count;
  Token ext_count;

  Dictionary() { count = 1;  ext_count = 1; }

  Token GetNext(int hint=0)
    {
      Token t = count*100+(hint%100);
      count++;
      assert(count<MAX_DICT);
      return t;
    }

  Token GetNextExt()
    {
      Token t = ext_count;
      ext_count++;
      assert(ext_count<100);
      return t;
    }

  Token Internalize(const char *s)
    {
      Token t = 0;
      ExternalToken et;
      et.Set(s);
      hash_ei::iterator it = ext_int.find(et);
      if (it == ext_int.end())
	{
	  t = GetNextExt();
	  ext_int[et] = t;
	  int_ext[t] = et;
	}
      else
	{
	  t = (*it).second;
	}
      return t;
    }

  string Externalize(Token t)
    {
      string s = "(null)";
      hash_ie::iterator it = int_ext.find(t);
      if (it != int_ext.end())
	{
	  s = (*it).second.Get();
	}
      else
	{
	  char buf[256];
	  sprintf(buf,"%ld", t);
	  s = buf;
	}
      return s;
    }

  Token Register(int x)
    {
      Token t = GetNext(x);
      State s;
      s.Set(t);
      dictionary[t] = s;
      numeric[x] = t;
      identity_strength[t] = 1.0;
      return t;
    }

  double GetStrength(Token t)
    {
      return identity_strength[t];
    }

  void SetStrength(Token t, double s)
    {
      identity_strength[t] = s;
    }

  Token Convert(int x)
    {
      Token t;
      hash_ii::iterator it = numeric.find(x);
      if (it!=numeric.end())
	{
	  t = (*it).second;
	}
      else
	{
	  t = Register(x);
	}
      return t;
    }

  Token Register(State s, int hint)
    {
      Token t = GetNext(hint);
      dictionary[t] = s;
      stateful[s] = t;
      return t;
    }

  int Find(Token src, State& dest)
    {
      State s;
      int ok = 1;
      hash_is::iterator it = dictionary.find(src);
      if (it!=dictionary.end())
	{
	  dest = (*it).second;
	}
      else
	{
	  dest = s;
	  ok = 0;
	}
      return ok;
    }

  int Find(const State& src, Token dest)
    {
      int ok = 1;
      hash_si::iterator it = stateful.find(src);
      if (it!=stateful.end())
	{
	  dest = (*it).second;
	}
      else
	{
	  dest = 0;
	  ok = 0;
	}
      return ok;
    }
};

Dictionary dictionary;


class Method
{
public:
  int len;
  int src;
  int valid;
  Method() { len = 0; src = 0; valid = 1; }

  int Next()
    {
      if (valid)
	{
	  len++;
	  if (len>MAX_CONTEXT)
	    {
	      len = 0;
	      //src++;
	      //if (src>1)
		{
		  valid = 0;
		}
	    }
	}
      return valid;
    }
};

class Context
{
public:
  Token token[MAX_CONTEXT];
  int count[MAX_CONTEXT];

  /*
  Token Predict(const Method &method)
    {
      return (method.src==0)?right:long_right;
    }
  */

  //#define NEW_COMPARE
  double Compare(const Context& alt, const Method &method)
    {
      double dist = 1;
      Token prev = 0;
      int diffs = 0;
      for (int i=0; i<method.len; i++)
	{
	  if (token[i] != prev)
	    {
	      prev = token[i];
	      diffs++;
	    }
	  if (token[i] == 0 || alt.token[i] == 0)
	    {
	      dist = 0;
	      break;
	    }
	  if (token[i] != alt.token[i])
	    {
#ifdef NEW_COMPARE
	      double s1 = dictionary.GetStrength(token[i]);
	      double s2 = dictionary.GetStrength(alt.token[i]);
	      dist *= (1-s1)*(1-s2);
	      if (dist<0.99)
		{
		  dist = 0;
		  break;
		}
#else
	      dist = 0;
	      break;
#endif
	    }
	}
#ifndef NEW_COMPARE
      dist *= diffs/(method.len+1.0);
#endif
      return dist;
    }
};

class History
{
public:
  Token token[MAX_TOKEN];
  int count[MAX_TOKEN];
  int fair_game[MAX_TOKEN];

  int token_top;

  History()
    {
      token_top = -1;
    }

  void Clear()
    {
      token_top = -1;
    }

  void AddQuiet(Token t, int game=1)
    {
      if (token_top>=0)
	{
	  if (token[token_top]!=t)
	    {
	      token_top++;
	      token[token_top] = 0;
	      count[token_top] = 0;
	      fair_game[token_top] = 0;
	    }
	}
      else
	{
	  token_top++;
	  token[token_top] = 0;
	  count[token_top] = 0;
	  fair_game[token_top] = 0;
	}
      assert(token_top<MAX_TOKEN);
      token[token_top] = t;
      count[token_top]++;
      fair_game[token_top] |= game;
    }

  void Add(Token t, int game=1)
    {
      AddQuiet(t,game);
      //printf("[ %d ]\n", t);
    }

  void GetHistogram()
    {
      printf("Histogram -- word frequencies\n");
      hash_ii hcount;
      float total = 0;
      float highest = 0;
      for (int i=0; i<=token_top; i++)
	{
	  int t = token[i];
	  int ct = count[i];
	  if (hcount.find(t) == hcount.end())
	    {
	      hcount[t] = 0;
	    }
	  hcount[t] += ct;
	  total += ct;
	  if (hcount[t]>highest)
	    {
	      highest = hcount[t];
	    }
	}
      if (total<1) total = 1;
      if (highest<1) highest = 1;
      for (hash_ii::iterator it=hcount.begin(); it!=hcount.end(); it++)
	{
	  Token t = (*it).first;
	  int ct = (*it).second;
	  double s = ct/highest;
	  dictionary.SetStrength(t,s);
	  printf("Word %s has count %d (strength %g)\n", 
		 YARPDictionary::GetText(t).c_str(), 
		 ct, dictionary.GetStrength(t));
	  
	}
    }

  void Read(const char *filename)
    {
      int ct = 0;
      ifstream fin(filename);
      fprintf(stderr,"[ ");
      while (!fin.eof())
	{
	  int t1;
	  char buf[256];
	  //fin >> t1;
	  int done = 0;
	  int working = 0;
	  int i = 0;
	  while (!done && i<(sizeof(buf)-1))
	    {
	      buf[i] = '\0';
	      fin.get(buf[i]);
	      //printf(">>> got character %d\n", buf[i]);
	      if (buf[i]==' '|| buf[i]=='\n' || buf[i]=='\t')
		{
		  if (working)
		    {
		      done = 1;
		    }
		}
	      else
		{
		  i++;
		  working = 1;
		}
	    }
	  buf[i] = '\0';
	  //printf(">>> read %s\n", buf);

	  t1 = dictionary.Internalize(buf);

	  if (!(fin.eof()))
	    {
	      Token t = dictionary.Convert(t1);
	      AddQuiet(t);
	      ct++;
	      fprintf(stderr,"%d ", token[token_top]);
	    }
	}
      fprintf(stderr,"]\n");
      fprintf(stderr,"Read %d token(s)\n", ct);
    }

  void SetContext(Context& con, int index)
    {
      for (int i=0; i<MAX_CONTEXT; i++)
	{
	  Token t = 0;
	  int c = 0;
	  if (index>=0 && index<=token_top)
	    {
	      t = token[index];
	      c = count[index];
	    }
	  else
	    {
	      t = 0;
	      c = 0;
	    }
	  con.token[i] = t;
	  con.count[i] = c;
	  index--;
	}
      /*
      printf("^^^ ");
      for (int i=0; i<MAX_CONTEXT; i++)
	{
	  printf("%d", con.token[i]);
	}
      printf(" ");
      for (int i=0; i<MAX_CONTEXT; i++)
	{
	  printf("%d", con.long_token[i]);
	}
      printf("\n");
      */
    }

  void Predict()
    {
      hash_id value;
      Context ref;
      SetContext(ref,token_top);
      Predict(ref,value,0,CurrentCount());
    }

  int CurrentCount()
    {
      int c = 0;
      if (token_top>=0)
	{
	  c = count[token_top];
	}
      return c;
    }

  void Predict(Context& ref, hash_id& value, int verbose=0, int offset=0)
    {
      Context alt;
      //SetContext(ref,token_top);
      Method m;
      int mid = 0;
      int current_count = offset; //CurrentCount();
      //      printf("(%d)\n", current_count);
      do {
	hash_id counts;
	double total_votes = 0;
	for (int i=0; i<token_top; i++)
	  {
	    if (fair_game[i])
	      {
		SetContext(alt,i);
		double result = ref.Compare(alt,m);
		if (result>0.0001)
		  {
		    int addr = token[i+1];
		    if (count[i]>current_count)
		      {
			//printf("back off\n");
			addr = token[i];
		      }
		    hash_id::iterator it = counts.find(addr);
		    if (it==counts.end())
		      {
			counts[addr] = 0;
		      }
		    counts[addr]+=result;
		    total_votes+=result;
		  }
	      }
	  }
	if (verbose) printf("<%d:%g:",mid,total_votes);
	for (hash_id::iterator it=counts.begin(); it!=counts.end(); it++)
	  {
	    int candidate = (*it).first;
	    double votes = (*it).second;
	    if (verbose) printf("(%d,%g)", candidate,votes);
	    hash_id::iterator it = value.find(candidate);
	    if (it==value.end())
	      {
		value[candidate] = 0;
	      }
	    double v = 0;
	    if (total_votes>2 || (total_votes>1 && total_votes==votes))
	      {
		//		v = votes/(total_votes+3.0);
		v = 3+log(votes);
		//v = votes/(total_votes+10.0);
		//v = votes;
		/*
		if (votes<=total_votes-3)
		  {
		    v /= 3;
		  }
		*/
	      }
	    else
	      {
		v = votes/100;
	      }
	    //v = votes;
	    value[candidate] += v;
	  }
	if (verbose) printf(">\n");
	mid++;
      } while (m.Next());
      if (verbose) printf("\n");
      for (hash_id::iterator it=value.begin(); it!=value.end(); it++)
	{
	    int candidate = (*it).first;
	    double votes = (*it).second;
	    if (verbose) printf("(%d,%g) ", candidate, votes);
	    
	}
      if (verbose) printf("\n");
    }


  int Convert(History& hist, YARPAutomaton& automaton)
    {
      hist.Clear();
      automaton.clear();

      hash_si states;
      hash_is validation;
      hash_is mapping;
      State prev_state;
      Token prev_token = 0;
      int need_repeat = 0;
      for (int i=0; i<=token_top; i++)
	{
	  int width = count[i];
	  //assert(width<5);
	  for (int j=1; j<=width; j++)
	    {
	      hash_id value;
	      Context ref;
	      SetContext(ref,i);
	      Predict(ref,value,0,j);
	      Token ibest = 0;
	      double vbest = -1;
	      for (hash_id::iterator it=value.begin(); it!=value.end(); it++)
		{
		  Token candidate = (*it).first;
		  double votes = (*it).second;
		  if (votes>vbest)
		    {
		      ibest = candidate;
		      vbest = votes;
		    }
		}
	      double vnorm = 0;
	      double factor = 3;
	      for (hash_id::iterator it=value.begin(); it!=value.end(); it++)
		{
		  Token candidate = (*it).first;
		  double votes = (*it).second;
		  if (votes>vbest/factor)
		    {
		      vnorm+=votes;
		    }
		}
	      double target = YARPRandom::ranf() * vnorm;
	      double vnorm2 = 0;
	      State state;
	      Token current_token = token[i];
	      state.Set(current_token);
	      int done = 0;
	      for (hash_id::iterator it=value.begin(); it!=value.end(); it++)
		{
		  Token candidate = (*it).first;
		  double votes = (*it).second;
		  if (votes>vbest/factor)
		    {
		      state.Add(candidate);
		      //printf("*");
		      vnorm2+=votes;
		      if (!done)
			{
			  if (vnorm2>=target)
			    {
			      ibest = candidate;
			      done = 1;
			    }
			}
		    }
		}
	      
	      hash_si::iterator it = states.find(state);
	      if (it==states.end())
		{
		  //printf("*");
		  states[state] = dictionary.Register(state,state.Get());
		}
	      else
		{
		  //states[state] = current_token;
		}
	      Token state_token = states[state];
	      
	      hash_is::iterator it_valid = validation.find(current_token);
	      if (it_valid!=validation.end())
		{
		  equal_state es;
		  if (!(es((*it_valid).second,state)))
		    {
		      //if (!need_repeat)
		      {
			//printf("!");
		      }
		      need_repeat = 1;
		    }
		}
	      validation[current_token] = state;
	      mapping[state_token] = state;
	      prev_state = state;
	      prev_token = state_token;
	      
	      //printf("%d ", ibest);
	      fprintf(stderr,"%d ", state_token);
	      //printf("(%d/%d) ", ibest, state.GetCount());
	      hist.AddQuiet(state_token,1);
	    }
	}
      fprintf(stderr,"\n");

      if (!need_repeat)
	{
	  fprintf(stderr,"Found automaton\n");

	  /*
	    Let's eliminate everything that is clearly off-loop
	   */
	  hash_ii ignore;
	  int scanning = 1;
	  int prunable = 0;
	  if (1)
	    {
	      hash_ii has_support;
	      scanning = 1;
	      prunable = 0;
	      
	      has_support[prev_state.Get()] = 1;
	      
	      while (scanning)
		{
		  scanning = 0;
		  for (hash_is::iterator it=mapping.begin(); 
		       it!=mapping.end(); it++)
		    {
		      Token t = (*it).first;
		      State s = (*it).second;
		      if (has_support.find(s.Get()) != has_support.end())
			{
			  for (int i=0; i<s.GetCount(); i++)
			    {
			      Token t2 = s.Get(i);
			      if (has_support.find(t2) == has_support.end())
				{
				  has_support[t2] = 1;
				  //printf("%d has support\n", t2);
				  scanning = 1;
				  prunable = 1;
				}
			    }
			}
		    }
		}
	      for (hash_is::iterator it=mapping.begin(); 
		   it!=mapping.end(); it++)
		{
		  Token t = (*it).first;
		  State s = (*it).second;
		  if (has_support.find(s.Get()) == has_support.end())
		    {
		      ignore[s.Get()] = 1;
		    }
		}

	    }

	  for (hash_is::iterator it=mapping.begin(); 
	       it!=mapping.end(); it++)
	    {
	      Token t = (*it).first;
	      State s = (*it).second;
	      if (ignore.find(s.Get())==ignore.end() || !prunable)
		{
		  if (0)
		    {
		      printf("%d:%s --> (%d) --> ", s.Get(), dictionary.Externalize(s.Get()%100).c_str(), s.Get()%100);
		      for (int i=0; i<s.GetCount(); i++)
			{
			  printf("%d:%s ", s.Get(i), dictionary.Externalize(s.Get(i)%100).c_str());
			}
		      printf("\n");
		    }
		  YARPAutomatonEntry entry;
		  entry.id = s.Get();
		  entry.emit = s.Get()%100;
		  for (int i=0; i<s.GetCount(); i++)
		    {
		      entry.targets.push_back(s.Get(i));
		    }
		  automaton[s.Get()] = entry;
		}
	    }
	  if (!prunable)
	    {
	      hash_ii has_previous;
	      for (hash_is::iterator it=mapping.begin(); it!=mapping.end(); it++)
		{
		  Token t = (*it).first;
		  State s = (*it).second;
		  for (int i=0; i<s.GetCount(); i++)
		    {
		      has_previous[s.Get(i)] = t;
		      //printf("%d has previous\n", s.Get(i), t);
		    }
		}

	      YARPAutomatonEntry entry;
	      entry.id = 0;
	      entry.emit = 0;
	      int add_entry = 0;
	      for (hash_is::iterator it=mapping.begin(); 
		   it!=mapping.end(); it++)
		{
		  Token t = (*it).first;
		  State s = (*it).second;
		  if (has_previous.find(s.Get()) == has_previous.end())
		    {
		      //printf("0:0 --> (0) --> %d:%s\n", s.Get(),
		      //dictionary.Externalize(s.Get()%100).c_str());
		      add_entry = 0;
		      entry.targets.push_back(s.Get());
		    }
		}
	      if (add_entry)
		{
		  automaton[entry.id] = entry;
		}
	    }
	}
      else
	{
	  fprintf(stderr,"Need iteration to find automaton\n");
	}
      return !need_repeat;
    }
};



template <class T>
T *assertion_protected(T *t)
{
  assert(t!=NULL);
  return t;
}

class YARPAutomatonMakerResources
{
public:
  History hist, hist2;
  
  YARPAutomaton automaton;
  
  YARPAutomatonMakerResources()
    {
    }

};

#define RES(x) (*((YARPAutomatonMakerResources*)(assertion_protected(x))))

void YARPAutomatonMaker::StartResources()
{
  if (resources==NULL)
    {
      resources = new YARPAutomatonMakerResources;
      assert(resources!=NULL);
    }
}


void YARPAutomatonMaker::StopResources()
{
  if (resources!=NULL)
    {
      delete (&RES(resources));
      resources = NULL;
    }
}

void YARPAutomatonMaker::Clear()
{
  RES(resources).hist.Clear();
  RES(resources).hist2.Clear();
}

void YARPAutomatonMaker::Add(const YARPToken& t)
{
  RES(resources).hist.Add(t,1);
}

void YARPAutomatonMaker::Add(const char *str)
{
  RES(resources).hist.Add(YARPDictionary::GetToken(str),1);
}

void YARPAutomatonMaker::GetHistogram()
{
  RES(resources).hist.GetHistogram();
}


bool YARPAutomatonMaker::Iterate()
{
  History& hist = RES(resources).hist;
  History& hist2 = RES(resources).hist2;
  YARPAutomaton& automaton = RES(resources).automaton;
  hist2.Clear();
  int result = !hist.Convert(hist2,automaton);
  //int result = RES(resources).hist.Convert(RES(s.resources).hist);
  if (result)
    {
      hist.Clear();
      for (int i=0; i<=hist2.token_top; i++)
	{
	  hist.token[i] = hist2.token[i];
	  hist.count[i] = hist2.count[i];
	  hist.fair_game[i] = hist2.fair_game[i];
	}
      hist.token_top = hist2.token_top;
    }
  return (result);
}


void YARPAutomatonMaker::Complete()
{
  while (Iterate()) {}
}


void YARPAutomatonMaker::GetAutomaton(YARPAutomaton& automaton)
{
  automaton = RES(resources).automaton;
}


YARPToken YARPDictionary::GetToken(const char *str)
{
  Token t = dictionary.Internalize(str);
  return (YARPToken) t;
}


std::string YARPDictionary::GetText(const YARPToken& loc)
{
  Token t = (Token)loc;
  return dictionary.Externalize(t);
}


std::string YARPDictionary::GetText(std::istream& is, int *ok = NULL)
{
  string s;
  int success = 0;

  int t1;
  char buf[256];
  //fin >> t1;
  int done = is.eof();
  int working = 0;
  int i = 0;
  while (!done && i<(sizeof(buf)-1) && !is.eof())
    {
      buf[i] = '\0';
      is.get(buf[i]);
      if (buf[i]==' ' || buf[i]=='\n' || buf[i]=='\r'||buf[i]=='\t'||is.eof())
	{
	  if (working)
	    {
	      done = 1;
	    }
	}
      else
	{
	  i++;
	  working = 1;
	}
    }
  buf[i] = '\0';
  //printf(">>> read %d %s\n", buf[0], buf);
  
  t1 = dictionary.Internalize(buf);

  if (working)
    {
      s = buf;
      success = 1;
    }

  if (ok!=NULL)
    {
      *ok = success;
    }
  if (!success)
    {
      s = "_notset_";
    }
  return s;
}


void YARPAutomaton::Show(ostream& os) const
{
  os << "BEGIN automaton" << endl;
  for (YARPAutomaton::const_iterator it = begin(); it != end(); it++)
    {
      unsigned int id = (*it).first;
      const YARPAutomatonEntry& entry = (*it).second;
      unsigned int emit = entry.emit;
      os << id << ":" << YARPDictionary::GetText(emit) << " --> ";
      os << "(" << YARPDictionary::GetText(emit) << ")";
      os << " --> ";
      for (list<YARPToken>::const_iterator it2 = entry.targets.begin();
	   it2 != entry.targets.end(); it2++)
	{
	  unsigned int id2 = (*it2);
	  YARPAutomaton::const_iterator it3 = find(id2);
	  if (it3!=end())
	    {
	      os << id2 << ":" 
		 << YARPDictionary::GetText((*it3).second.emit) << " ";
	    }
	}
      os << endl;
    }
  os << "END automaton" << endl;
}

ostream& operator<<(ostream& os, const YARPAutomaton& automaton)
{
  automaton.Show(os);
  return os;
}



void YARPSequence::Parse(YARPAutomaton& automaton, YARPSequence& target)
{
  YARPViterbi v;
  hash_map<YARPToken, YARPToken, hash<YARPToken>, equal_to<YARPToken> > order;
  hash_map<YARPToken, YARPToken, hash<YARPToken>, equal_to<YARPToken> > iorder;
  //printf(">>> size %d by %d\n", automaton.size(),seq.size());
  v.SetSize(automaton.size()+1,seq.size()+3);
  int ct = 1;
  v.BeginTransitions();
  v.AddTransition(0,0,999.0);
  for (YARPAutomaton::const_iterator it2 = automaton.begin();
       it2 != automaton.end(); it2++)
    {
      YARPToken id2 = (*it2).first;
      YARPToken emit = (*it2).second.emit;
      order[id2] = ct;
      iorder[ct] = id2;
      v.AddTransition(ct,ct,1);
      //printf("LINK %ld %ld %ld\n", ct, id2, emit);
      ct++;
    }
  v.EndTransitions();
  for (list<YARPToken>::const_iterator it = seq.begin(); it != seq.end(); it++)
    {
      YARPToken id = (*it);
      //printf(">>> %d\n", __LINE__);
      v.BeginTransitions();
      v.AddTransition(0,0,999.0);
      //printf(">>> %d\n", __LINE__);
      for (YARPAutomaton::const_iterator it2 = automaton.begin();
	   it2 != automaton.end(); it2++)
	{
	  //printf(">>> %d\n", __LINE__);
	  YARPToken id2 = (*it2).first;
	  const YARPAutomatonEntry& entry = (*it2).second;
	  YARPToken emit = entry.emit;
	  if (emit==id)
	    {
	      //printf(">> emit %ld\n", emit);
	      for (list<YARPToken>::const_iterator it3 = entry.targets.begin();
		   it3 != entry.targets.end(); it3++)
		{
		  //printf(">>> %d\n", __LINE__);
		  YARPToken id3 = (*it3);
		  v.AddTransition(order[id2],order[id3],1);
		}
	    }
	}
      v.EndTransitions();
    }
  //printf(">>> %d\n", __LINE__);
  v.CalculatePath();
  //printf(">>> %d\n", __LINE__);
  //v.ShowPath();
  //printf(">>> %d\n", __LINE__);
  target.seq.clear();
  for (int i=0; i<v.length()-1; i++)
    {
      //printf(">>> %d\n", __LINE__);
      target.seq.push_back(iorder[v(i)]);
    }
  //cout << "size " << target.seq.size() << " versus " << v.length()-1 << endl;
  cout << "****" << target << endl;
}



ostream& operator<<(ostream& os, const YARPSequence& ys)
{
  //printf(">>> %d\n", __LINE__);
  for (list<YARPToken>::const_iterator it = ys.seq.begin(); it!=ys.seq.end(); 
       it++)
    {
      //printf(">>> %d\n", __LINE__);
      os << (*it) << " ";
    }
  return os;
}
