#include "YARPAll.h"

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include <iomanip>

#include <vector>
#include <deque>
#include <hash_map>
using namespace std;

#include "YARPPort.h"
#include "YARPBottle.h"
#include "YARPTime.h"
#include "YARPRandomNumber.h"

#include "VisMatrix.h"
#include "RobotMath.h"
#define Matrix CVisDMatrix
#define Vector CVisDVector
#define Vector3 C3dVector

#ifdef __QNX__
#define for if (1) for
#endif

YARPInputPortOf<YARPBottle> in_bot;

#define FAUX_DIM 2

class FauxPos
{
public:
  Vector v;

  FauxPos(int sz = FAUX_DIM) : v(sz) { Zero(); }


  void Zero()
    {
      for (int i=0; i<FAUX_DIM; i++)
	{
	  v[i] = 0;
	}
    }

  void Randomize()
    {
      for (int i=0; i<FAUX_DIM; i++)
	{
	  v[i] = YARPRandom::Uniform()*2 - 1;
	}
    }

  double GetDistance(const FauxPos& alt)
    {
      Vector diff = v-alt.v;
      return diff.norm2();
    }
};

class JointID
{
public:
  int id1, id2;

  JointID() { id1 = id2 = 0; }

  JointID(int n_id1, int n_id2)
    { id1 = n_id1;  id2 = n_id2; }
};

struct equal_JointID
{
  bool operator()(const JointID& s1, const JointID& s2) const
    {
      return s1.id1 == s2.id1 && s1.id2 == s2.id2;
    }
};

struct hash_JointID
{
  size_t operator()(const JointID& et) const
    {
      hash<long int> h;
      return h(et.id1 + et.id2*65536);
    }
};

#define LONG_TIME 1e30

class JointState
{
public:
  int count;
  double shortest_time;
  double mean_time;
  double total_time;

  FauxPos pos;
  FauxPos effect;
  int effect_count;

  JointState()
    { count = 0;  shortest_time = LONG_TIME; }
};

typedef hash_map<JointID,JointState,hash_JointID,equal_JointID> hash_js;
typedef hash_map<int, int, hash<int>, equal_to<int> > hash_ii;



class TeljeEvent
{
public:
  double t;
  int id;
  int ct;
  vector<double> val;

  void Set(double n_time, int n_id)
    { t = n_time;  id = n_id;  ct = 0;  val.erase(val.begin(),val.end()); }

  void Add(double n_val)
    {
      val.push_back(n_val);
      ct++;
    }

  void Show() const
    {
      printf("*** Event %s (%d) at t=%g with %d values ", 
	     ybc_label(id),
	     id, t, ct);
      for (vector<double>::const_iterator it = val.begin();
	   it != val.end(); it++)
	{
	  double v = (*it);
	  printf("(%g) ", v);
	}
      printf("\n");
    }

  double GetTime() const { return t; }
  int GetID() const { return id; }
};

typedef deque<TeljeEvent> TeljeEvents;

class TeljeManager
{
public:
  TeljeEvents events;
  hash_js stats, stats_single;

  void Add(const TeljeEvent& ev)
    {
      //ev.Show();
      events.push_back(ev);
    }

  JointState& GetState(int id)
    {
      JointID jid(id,id);
      hash_js::iterator it = stats_single.find(jid);
      assert(it!=stats_single.end());
      return (*it).second;
    }

  void Update()
    {
      /*
      for (TeljeEvents::const_iterator it = events.begin();
	   it != events.end(); it++)
	{
	  //(*it).Show();
	}
      */

      for (hash_js::iterator it = stats.begin(); it!=stats.end(); it++)
	{
	  JointID& jid = (*it).first;
	  JointState& state = (*it).second;
	  int freq = state.count;
	  if (freq>3)
	    {
	      int freq1 = GetState(jid.id1).count;
	      int freq2 = GetState(jid.id2).count;
	      if (freq1 == freq || freq2 == freq)
		{
		  if (state.mean_time<0.5)
		    {
		      printf("%s:%d %s:%d (freq %d/%d/%d short %g usual %g)\n", 
			     ybc_label(jid.id1), jid.id1,
			     ybc_label(jid.id2), jid.id2, 
			     freq, freq1, freq2,
			     state.shortest_time,
			     state.mean_time);
		    }
		}
	    }
	}

      for (hash_js::iterator it = stats_single.begin(); 
	   it!=stats_single.end(); it++)
	{
	  JointState& state = (*it).second;
	  state.pos.Randomize();
	  state.effect.Zero();
	  state.effect_count = 0;
	}
      
      int kktop = 100;
      float factor = 1e10;
      for (int kk=0; kk<=kktop; kk++)
	{
	  printf("--\n");
	  for (hash_js::iterator it = stats.begin(); it!=stats.end(); it++)
	    {
	      JointID& jid = (*it).first;
	      JointState& state = (*it).second;
	      
	      int id1 = jid.id1, id2 = jid.id2;
	      JointID jid1(id1,id1), jid2(id2,id2);
	      JointState& state1 = stats_single[jid1];
	      JointState& state2 = stats_single[jid2];
	      double dist = state1.pos.GetDistance(state2.pos);
	      double setpoint = state.mean_time;
	      int ct = state.count;
	      int ct1 = state1.count;
	      int ct2 = state2.count;
	      if (ct1<ct) ct = ct1;
	      if (ct2<ct) ct = ct2;
	      if (kk==kktop)
		{
		  printf("Dist %d to %d is %g, should be %g (%d)\n", 
			 id1, id2, dist,
			 setpoint, ct);
		}
	      Vector fix = state2.pos.v - state1.pos.v;
	      double force = (dist-setpoint);
	      //force /= fabs(setpoint)+2;
	      /*
	      if (fabs(force)>0.01)
		{
		  force = 1/force;
		}
	      else
		{
		  force = 0;
		}
	      */
	      //fix *= log(ct+4);
	      //fix /= (dist+0.0001);
	      fix *= force;
	      fix *= ct;
	      fix *= ct;
	      //if (force>0)
		{
		  state1.effect.v += fix;
		  state1.effect_count++;
		  state2.effect.v -= fix;
		  state2.effect_count++;
		}
	    }
	  for (hash_js::iterator it = stats_single.begin(); 
	       it!=stats_single.end(); it++)
	    {
	      for (hash_js::iterator it2 = stats_single.begin(); 
		   it2!=stats_single.end(); it2++)
		{
		  JointState& state1 = (*it).second;
		  JointState& state2 = (*it2).second;
		  Vector fix = state2.pos.v - state1.pos.v;
		  double dist = fix.norm2();
		  if (fabs(dist)>0.0001)
		    {
		      fix /= dist*dist*dist;
		    }
		  else
		    {
		      fix *= 0;
		    }
		  fix *= 10;
		  fix *= -1;
		  state1.effect.v += fix;
		  state1.effect_count++;
		  state2.effect.v -= fix;
		  state2.effect_count++;
		}
	    }
	  factor /= 2;
	  if (factor<10) factor = 10;
	  for (hash_js::iterator it = stats_single.begin(); 
	       it!=stats_single.end(); it++)
	    {
	      JointState& state = (*it).second;
	      Vector& v0 = state.pos.v;
	      Vector& dv = state.effect.v;
	      int count = state.effect_count;
	      if (state.effect_count>0)
		{
		  //dv /= state.effect_count;
		  dv /= dv.norm2();
		  dv *= 0.1;
		  v0 += dv;
		}
	    }
	}

      int idx = 0;
      for (hash_js::iterator it = stats_single.begin(); 
	   it!=stats_single.end(); it++)
	{
	  JointState& state = (*it).second;
	  Vector& v = state.pos.v;
	  printf("TELJE_POINT %d ", (*it).first.id1);
	  for (int i=0; i<FAUX_DIM; i++)
	    {
	      printf("%g ", v[i]);
	    }
	  printf("\n");
	  idx++;
	}
      
    }

  TeljeEvents::reverse_iterator FirstWithinRange(double seconds, int frames)
    {
      double t = -1;
      int frame_ct = 0;
      //if (frames>0) frames++;
      TeljeEvents::reverse_iterator result = events.rbegin();
      for (TeljeEvents::reverse_iterator it = events.rbegin();
	   it != events.rend(); it++)
	{
	  double tl = (*it).GetTime();
	  if (t<0) t = tl;
	  if (frames>0)
	    {
	      frame_ct++;
	    }
	  if (frame_ct>frames || tl<t-seconds)
	    {
	      //result++;
	      break;
	    }
	  else
	    {
	      result = it;
	    }
	}
      return result; //.base();
    }

  void NoteDirectionalRelationship(TeljeEvent& ev1, TeljeEvent& ev2)
    {
      JointID id(ev1.GetID(),ev2.GetID());
      hash_js::iterator entry = stats.find(id);
      if (entry == stats.end())
	{
	  stats[id] = JointState();
	  entry = stats.find(id);
	}
      JointState& state = entry->second;
      state.count++;
      double dt = ev2.GetTime()-ev1.GetTime();
      double adt = fabs(dt);
      if (adt<state.shortest_time)
	{
	  state.shortest_time = adt;
	}
      state.total_time += dt;
      state.mean_time = state.total_time/state.count;
    }

  void NoteRelationship(TeljeEvent& ev1, TeljeEvent& ev2)
    {
      NoteDirectionalRelationship(ev1,ev2);
      /*
      if (ev1.GetID()!=ev2.GetID())
	{
	  NoteDirectionalRelationship(ev2,ev1);
	}
      */
    }


  void ApplyWindow(double seconds, int frames)
    {
      hash_ii map_seen;
      if (events.begin()!=events.end())
	{
	  //TeljeEvents::iterator first = FirstWithinRange(30,1000);
	  TeljeEvents::reverse_iterator first = 
	    FirstWithinRange(seconds,frames);
	  TeljeEvent& ref = events.back();
	  if (first!=events.rend())
	    {
	      first++;
	    }

	  {
	    int id1 = ref.GetID();
	    JointID jid(id1,id1);
	    hash_js::iterator entry = stats_single.find(jid);
	    if (entry == stats_single.end())
	      {
		stats_single[jid] = JointState();
		entry = stats_single.find(jid);
	      }
	    JointState& state = (*entry).second;
	    state.count++;
	  }


	  for (TeljeEvents::reverse_iterator it = events.rbegin();
	       it != first; it++)
	    {
	      //(*it).Show();
	      // should examine the relationship between
	      // *first and ref
	      if (&ref != &(*it))
		{
		  hash_ii::iterator before = map_seen.find((*it).GetID());
		  if (before==map_seen.end())
		    {
		      map_seen[(*it).GetID()] = 1;
		      NoteRelationship((*it),ref);
		      //printf(".");
		    }
		}
	    }
	  //printf("\n");
	}
    }
} manager;


void Apply(const TeljeEvent& ev)
{
  ev.Show();
  manager.Add(ev);
  manager.ApplyWindow(30,1000);
}

void Update()
{
  manager.Update();
  //printf("\n\n");
  //manager.Test();
}


#define LOG_FILE "log.txt"

void main()
{
  double starting_time = 0;
  {
    printf("Recovering from log file\n");
    ifstream fin(LOG_FILE);
    while (!fin.eof() && !fin.fail())
      {
	char buf[10000] = "";
	char buf2[10000] = "";
	fin.getline(buf,sizeof(buf));
	//printf(">>> %s\n", buf);
	TeljeEvent ev;
	double t = 0, v = 0;
	int idp = 0;
	//ev.Set(t,idp);
	int state = 0;
	if (!fin.eof())
	  {
	    istrstream sin(buf);
	    while (!sin.eof() && !sin.fail())
	      {
		sin.get(buf2,sizeof(buf2),' ');
		if (!sin.eof())
		  {
		    //printf("(%s) ", buf2);
		    sin.get();
		    switch (state)
		      {
		      case 0:
			idp = atoi(buf2);
			break;
		      case 1:
			t = atof(buf2);
			ev.Set(t,idp);
			if (t>starting_time)
			  {
			    starting_time = t;
			  }
			break;
		      default:
			v = atof(buf2);
			ev.Add(v);
			break;
		      }
		    state++;
		  }
	      }
	    //printf("\n");
	  }
	if (state>=2)
	  {
	    Apply(ev);
	  }
      }
    Update();
  }

#ifdef __LINUX__
  printf("TELJE stopping\n");
  //exit(0);
#endif

  if (starting_time>100)
    {
      starting_time += 1000;
    }

  ofstream fout(LOG_FILE, ios::app);
  double start = YARPTime::GetTimeAsSeconds();
  in_bot.Register("/log/i:bot");
  printf("TELJE is in operation\n");
  while (1)
    {
      in_bot.Read();
      double now = YARPTime::GetTimeAsSeconds();
      printf("TELJE got a message.\n");
      YARPBottleIterator it(in_bot.Content());
      it.Display();
      it.Rewind();
      int id = it.ReadVocab();
      if (id!=YBC_LOG_PERCEPT)
	{
	  printf("Message ignored\n");
	}
      else
	{
	  int idp = it.ReadVocab();
	  int ct = 0;
	  double t = now-start+starting_time;
	  fout << idp << " " << setprecision(30) << t << " ";
	  TeljeEvent ev;
	  ev.Set(t,idp);
	  while (it.More())
	    {
	      double val = it.ReadFloat();
	      fout << val << " ";
	      printf("percept %s (%d) index %d value %g     time %g\n",
		     ybc_label(idp),
		     idp, ct, val, t);
	      ct++;
	      ev.Add(val);
	    }
	  fout << endl;
	  fout.flush();
	  Apply(ev);
	}
    }
}

