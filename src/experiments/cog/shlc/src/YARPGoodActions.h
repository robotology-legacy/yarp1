//
// YARPGoodActions.h
//
//


#ifndef __YARPGoodActionsh__
#define __YARPGoodActionsh__

#include "vector.h"
#include "stdio.h"
#include "math.h"

#include "classes.h"
#include "YARPFlowTracker.h"

//
//
//
class YARPGoodUnit
{
public:
	double m_theta, m_phi;	// real gaze.
	double m_torso;			// torso position.
	int m_action_no;		// generated action.	
	double m_reinforcer;	//
	int m_merges;

	YARPGoodUnit ();
	YARPGoodUnit (const YARPGoodUnit& u);
	~YARPGoodUnit ();
	void operator= (const YARPGoodUnit& u);

	int Save (FILE *fp);
	int Load (FILE *fp);

	int Merge (const YARPGoodUnit& u);
};

// a list for similar theta, phi but different torso values.
class YARPGoodUnitList
{
protected:	
	vector<YARPGoodUnit> m_vector;
	typedef vector<YARPGoodUnit>::iterator POSITION;
	double m_prec_torso;
	
public:
	double m_ave_theta;
	double m_ave_phi;
	POSITION m_last_nearest;
	double m_mindist;
	int m_entries;

	YARPGoodUnitList (void);
	YARPGoodUnitList (const YARPGoodUnitList& u);
	YARPGoodUnitList (double prec_torso);
	~YARPGoodUnitList ();
	void operator= (const YARPGoodUnitList& u);

	inline void SetPrecision (double p) { m_prec_torso = p; }

	int SearchNearest (double torso);
	int AddToNearest (const YARPGoodUnit& u);
	inline YARPGoodUnit& GetNearest (void) { return *m_last_nearest; }

	int GetBest (YARPGoodUnit& u, double& score);

	int Save (FILE *fp);
	int Load (FILE *fp);
};

// a list for all possible theta, phi of lists for value of torso.
class YARPGoodActionMap : public vector<YARPGoodUnitList>
{
protected:
	typedef vector<YARPGoodUnitList>::iterator POSITION;

	double m_precision_theta;
	double m_precision_phi;
	double m_precision_torso;
	double m_mindist;
	double m_mindist_theta;
	double m_mindist_phi;
	POSITION m_last_nearest;

public:
	YARPGoodActionMap (void);
	YARPGoodActionMap (double prec_theta, double prec_phi, double prec_torso);
	~YARPGoodActionMap ();

	inline void SetPrecision (double pt, double pp, double tt)
	{
		m_precision_theta = pt;
		m_precision_phi = pp;
		m_precision_torso = tt;
	}

	int SearchNearest (double theta, double phi);
	YARPGoodUnitList& GetNearest (void) { return *m_last_nearest; }
	int AddToNearest (const YARPGoodUnit& u);
	inline int Empty() { return empty(); }

	int Size (void);

	int Save (char *filename);
	int Load (char *filename);
};


class YARPSecretProcessing;

//
//
// Not that clean and neat!
//
//
class YARPGoodActionMapArray
{
public:
	YARPGoodActionMap *m_goodaction;
	YARPFlowTracker *m_secret;
	int m_size;
	

	YARPGoodActionMapArray (int size, YARPFlowTracker *secret) 
		: m_size(size),
		  m_secret(secret) 
	{
		m_goodaction = new YARPGoodActionMap[m_size];
		assert (m_goodaction != NULL);

		for (int i = 0; i < m_size; i++)
			m_goodaction[i].SetPrecision (5.0*degToRad, 5.0*degToRad, 500.0);
	}
	
	~YARPGoodActionMapArray () 
	{
		if (m_goodaction != NULL)
		{
			delete[] m_goodaction;
			m_goodaction = NULL;
		}
	}

	int BuildActionMap (int ret);
	int QueryActionMap (float theta, float phi, float& torso, int& action_no);

	int Save (char *filename);
	int Load (char *filename);
};


#endif