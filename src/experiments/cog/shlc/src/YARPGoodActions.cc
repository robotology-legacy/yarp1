//
// YARPGoodActions.cc
//
//

#include "YARPGoodActions.h"



//
//
// YARPGoodUnit
//
//
YARPGoodUnit::YARPGoodUnit ()
{
//	printf ("YARPGoodUnit - default constructor\n");
	m_theta = 0;
	m_phi = 0;
	m_torso = 0;
	m_action_no = 0;
	m_reinforcer = 0;

	m_merges = 0;
}

YARPGoodUnit::YARPGoodUnit (const YARPGoodUnit& u)
{
//	printf ("YARPGoodUnit - copy constructor\n");
	m_theta = u.m_theta;
	m_phi = u.m_phi;
	m_torso = u.m_torso;
	m_action_no = u.m_action_no;
	m_reinforcer = u.m_reinforcer;

	m_merges = u.m_merges;
}

YARPGoodUnit::~YARPGoodUnit () {}

void YARPGoodUnit::operator= (const YARPGoodUnit& u)
{
//	printf ("YARPGoodUnit - operator=\n");

	m_theta = u.m_theta;
	m_phi = u.m_phi;
	m_torso = u.m_torso;
	m_action_no = u.m_action_no;
	m_reinforcer = u.m_reinforcer;

	m_merges = u.m_merges;
}

int YARPGoodUnit::Save (FILE *fp)
{
	fprintf (fp, "theta, phi : %lf %lf\n", m_theta, m_phi);
	fprintf (fp, "torso : %lf\n", m_torso);
	fprintf (fp, "action no : %d\n", m_action_no);
	fprintf (fp, "reinforcer : %lf\n", m_reinforcer);
	fprintf (fp, "merges : %d\n", m_merges);

	return 0;
}

int YARPGoodUnit::Load (FILE *fp)
{
	fscanf (fp, "theta, phi : %lf %lf\n", &m_theta, &m_phi);
	fscanf (fp, "torso : %lf\n", &m_torso);
	fscanf (fp, "action no : %d\n", &m_action_no);
	fscanf (fp, "reinforcer : %lf\n", &m_reinforcer);
	fscanf (fp, "merges : %d\n", &m_merges);
	
	return 0;
}

int YARPGoodUnit::Merge (const YARPGoodUnit& u)
{
	assert (u.m_merges == 0);

	m_merges++;
	
	m_theta = (m_theta * (m_merges) + u.m_theta)/(m_merges+1);
	m_phi = (m_phi * (m_merges) + u.m_phi)/(m_merges+1);
	
	assert (m_action_no == u.m_action_no);

	m_reinforcer = (m_reinforcer * (m_merges) + u.m_reinforcer)/(m_merges+1);
	return 0;
}

//
//
// YARPGoodUnitList
//
//
int YARPGoodUnitList::Save (FILE *fp)
{
	fprintf (fp, "YARPGoodUnitList\n");
	fprintf (fp, "average theta : %lf\n", m_ave_theta);
	fprintf (fp, "average phi : %lf\n", m_ave_phi);
	fprintf (fp, "precision torso : %lf\n", m_prec_torso);
	fprintf (fp, "entries : %d\n", m_entries);
	fprintf (fp, "actual size : %d\n", m_vector.size());

	POSITION index = m_vector.begin();
	int i = 0;
	while (index != m_vector.end())
	{
		fprintf (fp, "Unit : %d\n", i);
		index->Save (fp);
		index++;
		i++;
	}
	return 0;
}

int YARPGoodUnitList::Load (FILE *fp)
{
	fscanf (fp, "YARPGoodUnitList\n");
	fscanf (fp, "average theta : %lf\n", &m_ave_theta);
	fscanf (fp, "average phi : %lf\n", &m_ave_phi);
	fscanf (fp, "precision torso : %lf\n", &m_prec_torso);
	fscanf (fp, "entries : %d\n", &m_entries);

	int tmp;
	fscanf (fp, "actual size : %d\n", &tmp);
	m_vector.erase (m_vector.begin(), m_vector.end());

	m_mindist = -1;
	m_last_nearest = m_vector.begin();

	int i;
	for (i = 0; i < tmp; i++)
	{
		int x;
		fscanf (fp, "Unit : %d\n", &x);
		YARPGoodUnit u;
		u.Load (fp);
		m_vector.push_back(u);
	}
	return 0;
}

YARPGoodUnitList::YARPGoodUnitList (void)
{
//	printf ("YARPGoodUnitList - default constructor\n");
	m_prec_torso = 0;
	m_ave_theta = 0;
	m_ave_phi = 0;

	m_last_nearest = m_vector.begin();
	m_mindist = -1;

	m_entries = 0;
}

YARPGoodUnitList::YARPGoodUnitList (const YARPGoodUnitList& u)
{
//	printf ("YARPGoodUnitList - copy constructor\n");
	m_prec_torso = u.m_prec_torso;
	m_ave_theta = u.m_ave_theta;
	m_ave_phi = u.m_ave_phi;

	m_last_nearest = u.m_last_nearest;
	m_mindist = u.m_mindist;

	m_entries = u.m_entries;
	m_vector = u.m_vector;

//	YARPGoodUnitList& v = (YARPGoodUnitList&)u;
//
//	POSITION index = v.m_vector.begin();
//	while (!v.m_vector.end())
//	{
//		m_vector.push_back(*index);
//		index++;
//	}
}

YARPGoodUnitList::YARPGoodUnitList (double prec_torso)
{
//	printf ("YARPGoodUnitList - 1 param constructor\n");

	m_prec_torso = prec_torso;
	m_ave_theta = 0;
	m_ave_phi = 0;

	m_last_nearest = m_vector.begin();
	m_mindist = -1;

	m_entries = 0;
}

YARPGoodUnitList::~YARPGoodUnitList ()
{
}

void YARPGoodUnitList::operator= (const YARPGoodUnitList& u)
{
//	printf ("YARPGoodUnitList - operator=\n");

	m_prec_torso = u.m_prec_torso;
	m_ave_theta = u.m_ave_theta;
	m_ave_phi = u.m_ave_phi;

	m_last_nearest = u.m_last_nearest;
	m_mindist = u.m_mindist;

	m_entries = u.m_entries;

	m_vector.erase (m_vector.begin(), m_vector.end());
	m_vector = u.m_vector;

	//YARPGoodUnitList& v = (YARPGoodUnitList&)u;

	//POSITION index = v.m_vector.begin();
	//while (!v.m_vector.end())
	//{
	//	m_vector.push_back(*index);
	//	index++;
	//}
}

int YARPGoodUnitList::AddToNearest (const YARPGoodUnit& u)
{
	if (m_vector.empty())
	{
//		printf ("YARPGoodUnitList - AddToNearest: list is empty\n");
		m_vector.push_back (u);

		m_ave_theta = u.m_theta;
		m_ave_phi = u.m_phi;

		m_entries = 1;
	}
	else
	{
		// I know already it's !empty
		SearchNearest(u.m_torso);
		if (m_mindist < m_prec_torso)
		{
//			printf ("YARPGoodUnitList - AddToNearest: merging unit\n");
			m_last_nearest->Merge (u);
		}
		else
		{
//			printf ("YARPGoodUnitList - AddToNearest: adding unit\n");
			m_vector.push_back(u);
		}

		m_entries++;
		m_ave_theta = (m_ave_theta*(m_entries-1)+u.m_theta)/m_entries;
		m_ave_phi = (m_ave_phi*(m_entries-1)+u.m_phi)/m_entries;
	}

	return 0;
}

int YARPGoodUnitList::SearchNearest (double torso)
{
	if (m_vector.empty())
	{	
//		printf ("YARPGoodUnitList - SearchNearest: list is empty\n");
		m_last_nearest = m_vector.begin();
		m_mindist = -1;
		return -1;
	}

	POSITION index = m_vector.begin();
	m_mindist = fabs (index->m_torso - torso);
	m_last_nearest = index;

	while (index != m_vector.end())
	{
		double d = fabs (index->m_torso - torso);
		if (d < m_mindist)
		{
			m_mindist = d;
			m_last_nearest = index;
		}
		index++;
	}

//	printf ("YARPGoodUnitList - SearchNearest: mindist %lf\n", m_mindist);
	return 0;
}

int YARPGoodUnitList::GetBest (YARPGoodUnit& u, double& score)
{
	if (m_vector.empty())
	{
//		printf ("YARPGoodUnitList - GetBest: empty list\n");
		score = 0;
		return -1;
	}
	else
	{
		double best_score = 0;
		POSITION index = m_vector.begin();
		while (index != m_vector.end())
		{
			if (index->m_reinforcer > best_score) /** index->m_merges*/
			{
				best_score = index->m_reinforcer;
				u = *index;
			}
			index++;
		}

		if (best_score == 0)
		{
//			printf ("YARPGoodUnitList - GetBest: score = 0 must be a bug %lf\n", best_score);
			score = 0;
			return -1;
		}

//		printf ("YARPGoodUnitList - GetBest: found something with score %lf\n", best_score);
		score = best_score;
		return 0;
	}
}

//
//
// YARPGoodActionMap
//
//
int YARPGoodActionMap::Save (char *filename)
{
	FILE *fp = fopen (filename, "w");
	if (fp == NULL)
	{
		printf ("YARPGoodActionMap: save to %s failed\n", filename);
		return -1;
	}

	fprintf (fp, "YARPGoodActionMap %s\n", filename);
	fprintf (fp, "precision theta : %lf\n", m_precision_theta);
	fprintf (fp, "precision phi : %lf\n", m_precision_phi);
	fprintf (fp, "size : %d\n", size());

	POSITION index = begin();
	while (index != end())
	{
		index->Save (fp);
		index ++;
	}

	fclose (fp);
	return 0;
}

int YARPGoodActionMap::Load (char *filename)
{
	FILE *fp = fopen (filename, "r");
	if (fp == NULL)
	{
		printf ("YARPGoodActionMap: load from %s failed\n", filename);
		return -1;
	}

	char buf[512];
	sprintf (buf, "YARPGoodActionMap %s\n\0", filename);
	fscanf (fp, buf);
	fscanf (fp, "precision theta : %lf\n", &m_precision_theta);
	fscanf (fp, "precision phi : %lf\n", &m_precision_phi);
	int size;
	fscanf (fp, "size : %d\n", &size);

	erase (begin(), end());

	int i;
	for (i = 0; i < size; i++)
	{
		YARPGoodUnitList l;
		l.Load (fp);
		push_back(l);
	}

	fclose (fp);
	return 0;
}

YARPGoodActionMap::YARPGoodActionMap (void)
{
	m_precision_theta = 0.0;
	m_precision_phi = 0.0;

	m_mindist = -1;
	m_mindist_theta = -1;
	m_mindist_phi = -1;

	m_last_nearest = begin();
}

YARPGoodActionMap::YARPGoodActionMap(double prec_theta, double prec_phi, double prec_torso)
{
	m_precision_theta = prec_theta;
	m_precision_phi = prec_phi;
	m_precision_torso = prec_torso;

	m_mindist = -1;
	m_mindist_theta = -1;
	m_mindist_phi = -1;

	m_last_nearest = begin();
}

YARPGoodActionMap::~YARPGoodActionMap() 
{
}


int YARPGoodActionMap::AddToNearest (const YARPGoodUnit& u)
{
	if (empty())
	{
//		printf ("YARPGoodActionMap - AddToNearest: list is empty\n");
		YARPGoodUnitList l(m_precision_torso);
		l.AddToNearest (u);
		push_back (l);
	}
	else
	{
		// I know already it's !empty
		SearchNearest(u.m_theta, u.m_phi);
		if (m_mindist_phi < m_precision_phi &&
			m_mindist_theta < m_precision_theta)
		{
//			printf ("YARPGoodActionMap - AddToNearest: found something close enough - existing unit\n");
			// something to add to within the precision thresholds.
			m_last_nearest->AddToNearest (u);
		}
		else
		{
//			printf ("YARPGoodActionMap - AddToNearest: nothing close enough - creating unit\n");
			// nothing close enough,
			YARPGoodUnitList l(m_precision_torso);
			l.AddToNearest (u);
			push_back (l);
		}
	}

	return 0;
}

int YARPGoodActionMap::Size (void)
{
	if (empty())
	{
		return 0;
	}
	else
	{
		int size = 0;
		POSITION index = begin();
		while (index != end())
		{
			size += index->m_entries;
			index++;
		}

		return size;
	}
}

int YARPGoodActionMap::SearchNearest (double theta, double phi)
{
	if (empty())
	{
//		printf ("YARPGoodActionMap - SearchNearest: list is empty!");
		m_last_nearest = begin();
		m_mindist = -1;
		return -1;
	}

	POSITION index = begin();
	m_mindist_theta = fabs (index->m_ave_theta - theta);
	m_mindist_phi = fabs (index->m_ave_phi - phi);
	m_mindist = sqrt (m_mindist_phi * 
					  m_mindist_phi + 
					  m_mindist_theta * 
					  m_mindist_theta);
	m_last_nearest = index;

	while (index != end())
	{
		double d = sqrt ((index->m_ave_phi - phi) * 
					  (index->m_ave_phi - phi) + 
					  (index->m_ave_theta - theta) * 
					  (index->m_ave_theta - theta));

		if (d < m_mindist)
		{
			m_mindist = d;
			m_mindist_theta = fabs (index->m_ave_theta - theta);
			m_mindist_phi = fabs (index->m_ave_phi - phi);
			m_last_nearest = index;
		}
		index++;
	}

//	printf ("YARPGoodActionMap - SearchNearest: found entry distance %lf\n", m_mindist);

	return 0;
}




//
//
// YARPGoodActionMapArray
//
//

int
YARPGoodActionMapArray::BuildActionMap (int ret)
{
	// get: 
	//	theta and phi
	//	torso
	//	action_no
	//	reinforcer

	int start = 0;
	int stop = 0;
	CVisDVector *phead;
	CVisDVector *parm;
	int contact = 0;
	m_secret->GetMotorData (start, stop, &phead, &parm, contact);	

	int action;
	m_secret->GetActionID (action);

	CogGaze gaze;
	JointPos head;
	ArmJoints arm;

	int i;
	for (i = 1; i <= COGHeadJoints; i++)
		head(i) = phead[contact](i);

	for (i = 1; i <= COGArmJoints; i++)
		arm.j[i-1] = parm[contact](i);

	gaze.Apply (head, arm);

	double theta = gaze.theta_middle;
	double phi = gaze.phi_middle;
	double torso = arm.j[2];

	// the reinforcement is a measure of the distance of the object (as computed from poking) 
	//	from the fovea.
	//
	CVisDVector v1(3);
	CVisDVector v2(3);
	CVisDVector i1(2);
	CVisDVector i2(2);
	v1 = 0;
	v2 = 0;
	i1 = 0;
	i2 = 0;
	m_secret->GetDisplacement(v1, v2, i1, i2);
	double reinforcer = 0.0;
	if (ret < 0.0)
	{
		reinforcer = 0.0;
	}
	else
	{
		printf ("BuildActionMap: object at %lf %lf\n", i1(1), i1(2));
		i1 -= i2;
		printf ("BuildActionMap: delta %lf %lf\n", i1(1), i1(2));

		double dist = i1.norm2();
		
		//const double sigma2 = 200*200;
		//reinforcer = 1.0/sqrt(2*pi*sigma2)*exp(-dist*dist/(2*sigma2));
		reinforcer = dist;
		printf ("BuildActionMap: reinforcer %lf\n", reinforcer);
	}

	// store in map.
	if (action < 0 || action >= m_size)
	{
		printf ("buildactionmap: action %d is not recognized\n", action);
		return -1;
	}

	YARPGoodUnit u;
	u.m_theta = theta;
	u.m_phi = phi;
	u.m_torso = torso;
	u.m_action_no = action;
	u.m_reinforcer = reinforcer;
	u.m_merges = 0;		// must be =0 when adding.

	m_goodaction[action].AddToNearest (u);

	return 0;
}

#define ACTION 3

int 
YARPGoodActionMapArray::QueryActionMap (float theta, float phi, float& torso, int& action_no)
{
	torso = 0;
	action_no = -1;
	double best_score = 0;


#if 0

	// if enough samples search the map.
	YARPGoodUnit u;

	int i = ACTION;

	int ret = m_goodaction[i].SearchNearest (theta, phi);
	if (ret >= 0)
	{
		double score = 0;
		m_goodaction[i].GetNearest().GetBest(u, score);
		printf ("QueryActionMap: score %lf\n", score);

		if (score > best_score)
		{
			torso = u.m_torso;
			action_no = u.m_action_no;
			best_score = score;
		}
	}
#endif

#if 0
	for (int i = 0; i < m_size; i++)	
	{
		if (m_goodaction[i].Size() > 50)
		{
			m_goodaction[i].SearchNearest (theta, phi);
			double score = 0;
			m_goodaction[i].GetNearest().GetBest(u, score);
			printf ("QueryActionMap: score %lf\n", score);

			if (score > best_score)
			{
				torso = u.m_torso;
				action_no = u.m_action_no;
				best_score = score;
			}
		}
		else
		{
			printf ("QueryActionMap: action map %d has NOT more than 50 samples - skipped\n", i);
		}
	}	
#endif

	printf ("QueryActionMap: torso %f, action %d\n", torso, action_no);

	return 0;
}

int
YARPGoodActionMapArray::Save (char *filename)
{
	char buf[512];
	for (int i = 0; i < m_size; i++)
	{
		sprintf (buf, "%s_%d.txt\0", filename, i);
		m_goodaction[i].Save (buf);
	}

	return 0;
}

int
YARPGoodActionMapArray::Load (char *filename)
{
	char buf[512];
	for (int i = 0; i < m_size; i++)
	{
		sprintf (buf, "%s_%d.txt\0", filename, i);
		m_goodaction[i].Load (buf);
	}

	return 0;
}


