//
// recognizer.cpp
//	- June 2001.
//

#include <sys/kernel.h>
#include <iostream.h>

// image services.
#include "YARPImage.h"
#include "YARPFilters.h"
#include "YARPClrHistogram.h"
#include "YARPHmm.h"
#include "YARPFftFeatures.h"
// som.
#include "Som.h"

#include "temporary/YARPConverter.h"	// YARP conversion (old)

// Remember to include the new version first!
#include <VisDMatrix.h>

// still old stuff.
#include <SomSendable.h>

// Old stuff.
#include "Image.h"
#include "ImageIO.h"
#include "PortOf.h"

#include <conf/tx_data.h>
#include <conf/lp_layout.h>

InputPortOf<GenericImage> in;
InputPortOf<JointSpeed> in_speed;
InputPortOf<JointPos> in_pos;

OutputPortOf<GenericImage> out;
OutputPortOf<GenericImage> out_h;
OutputPortOf<IntPosData> out_win;
OutputPortOfSendable<SomSendable> map;

char *my_name = "/recognizer";

char in_img_name[256];
char in_speed_name[256];
char in_pos_name[256];

char out_img_name[256];
char out_histo_name[256];
char out_map_name[256];
char out_win_name[256];

// parses command line.
void ParseCommandLine(int argc, char *argv[])
{
	int i;

	for(i=1;i<argc;i++)
	{
		switch(argv[i][0])
		{
			case '-':
				switch(argv[i][1])
				{
					case 'h':
					case '-':
						cout << endl;
						cout << my_name << " argument:" << endl;
						cout << endl;
						exit(0);
						break;

					default:
						break;
				}
				break;
			case '+':
				my_name = &argv[i][1];
				break;
			default:
				break;
		}
	}

	sprintf (in_img_name, "%s/in/img", my_name); 
	sprintf (in_speed_name, "%s/in/speed", my_name); 
	sprintf (in_pos_name, "%s/in/pos", my_name); 

	sprintf (out_img_name, "%s/out/img", my_name); 
	sprintf (out_histo_name, "%s/out/histo", my_name); 
	sprintf (out_map_name, "%s/out/map", my_name); 
	sprintf (out_win_name, "%s/out/win", my_name); 
}

const int period_ms = 40;
const int memory_size = 2000;

const int SOM_X = 12;
const int SOM_Y = 8;

// HMM related constants.
const int MAXOBJECTS = 10;
const int T = 100;
const int NEX = 30;		// it was 100. too long to wait.
const int NSTATES = 10;
const int M = 1;

const int FEATURESIZE = 104;

// TODO: this is still thread unsafe.

class CSomThread : public CThreadImpl
{
protected:
	CVisDVector m_training_set[memory_size];
	int m_used_memory;
	CVisDVector m_sample;

	YARPSom m_som;
	int m_som_size;
	typedef enum { SOM_One, SOM_Two, SOM_Ended, SOM_CollectData } SOM_Status;
	SOM_Status m_stage;

	int m_steps;
	CVisDVector m_winner;
	IntPosData m_winxy;
	int m_last_entry;


	////////////////////////////////////////////////////////////////
	// object recognition.
	YARPMixtureHmm *m_hmm_objects;	// array of HMMs.
	YARPSom m_local_som;			// som for HMM initialization.

	CVisDVector **m_mu;				// current initialization vector.
	CVisDVector **m_observation;	// current training set.
	int m_machine;					// current machine number.
	bool m_new_good_sample;			// new sample to be added.
	int m_nex;						// global indexes.
	int m_T;

public:
	CSomThread (int size, CVisDVector& min, CVisDVector& max)
		: CThreadImpl ("SomThread", period_ms)
	{
		m_som_size = size; // dim of the space.
		m_som.Init (size, SOM_X, SOM_Y,
				    10000, 0.05, 10,
					100000, 0.02, 3,
					min, max);

		m_som.SetVerbosity(0);
		m_som.SetSnapshotInfo ("recognizer.dat", 1000);

		m_winner.Resize (size);
		m_winner = 0;
		m_winxy.x = 0;
		m_winxy.y = 0;
		m_last_entry = 0;

		m_stage = SOM_One;

		// prepare the training set.
		for (int i = 0; i < memory_size; i++)
		{
			m_training_set[i].Resize (size);
			m_training_set[i] = 0;
		}

		m_used_memory = 0;

		m_sample.Resize(size);
		m_sample = 0;

		m_steps = 0;

		// channel initialization.
		map.SetName (out_map_name);
		out_win.SetName (out_win_name);

		// allocate memory within the SomSendable.
		map.Sendable().Allocate (size, SOM_X, SOM_Y);

	
		// alloc hmm stuff.
		YARPRandom::Seed (time(NULL));

		m_hmm_objects = new YARPMixtureHmm[MAXOBJECTS];
		assert (m_hmm_objects != NULL);

		// initialization.
		m_mu = AllocVector (NSTATES, M, size);
		m_observation = AllocVector (NEX, T, size);

		// current machine.
		m_machine = -1;

		//
		m_local_som.Init (size, NSTATES, M,
		  10000, 0.05f, 10,
		  100000, 0.02f, 3,
		  min, max);	

		m_new_good_sample = false;

		m_nex = 0;
		m_T = 0;
	}

	virtual ~CSomThread ()
	{
		map.Sendable().Free();

		FreeVector (m_mu, NSTATES);
		FreeVector (m_observation, NEX);

		if (m_hmm_objects != NULL)
			delete[] m_hmm_objects;
	}

	void DoInit (void *)
	{
		m_som.InitTrain1 ();
		m_steps = 0;
	}

	// TODO. Add display of winning unit.
	void DoLoop (void *)
	{
		switch (m_stage)
		{
		case SOM_One:
			{
				Lock ();
				// first gross ordering stage
				// get a random sample if m_used_memory > 150.
				// it was 1500 instead of 150.
				if (m_used_memory > 150)
				{
					int index = int((double(rand()) / RAND_MAX) * m_used_memory);
					if (index >= m_used_memory)
						index = m_used_memory-1;
					m_sample = m_training_set[index];

					int result = m_som.Train1 (m_sample, 1, 0, 0);

					m_steps ++;
					if (result != 0)
					{
						// end of stage 1.
						m_som.EndTrain1 ();
						//m_som.SaveMapPlanes ("end_stage1.cod");
						m_som.InitTrain2 ();
						m_stage = SOM_Two;
					}
				}

				Unlock ();
			}
			break;

		case SOM_Two:
			{
				Lock ();

				// fine tuning
				// it was 1500 instead of 150.
				if (m_used_memory > 150)  // it should always be > 150!
				{
					int index = int((double(rand()) / RAND_MAX) * m_used_memory);
					if (index >= m_used_memory)
						index = m_used_memory-1;
					m_sample = m_training_set[index];

					int result = m_som.Train2 (m_sample, 1, 0, 0);

					m_steps++;
					if (result != 0)
					{
						// end of stage 2.
						m_som.EndTrain2 ();
						//m_som.SaveMap ("recognizer_map.cod");
						//m_som.SaveMapPlanes ("excel_map.cod");
						m_stage = SOM_Ended;
					}
				}

				Unlock ();
			}
			break;

		default:
		case SOM_Ended:
			{
				// already trained
				m_steps++;
			}
			break;

		case SOM_CollectData:
			{
				// data collection for the HMM.
				if (m_new_good_sample)
				{
					m_new_good_sample = false;
					m_observation[m_nex][m_T] = m_training_set[m_last_entry];
					m_T ++;

					if (m_T > T)
					{
						printf ("New line of data started\n");
						printf ("line %d out of %d\n", m_nex+1, NEX);
						m_T = 1;
						m_nex ++;
						if (m_nex > NEX)
						{
							printf ("Data buffer is now full\n");
							m_stage = SOM_Ended;
						}
					}
				}
			}
			break;
		}

		Lock ();
		// write the all map status to port.
		if ((m_steps % 50) == 0)
		{
			// read data from som.
			if (map.Sendable().codes == NULL)
			{
				map.Sendable().Allocate (m_som_size, 12, 8);
			}
			m_som.GetCodes (map.Sendable().codes[0]);
			map.Write ();
		}

		// send the winner position.
		out_win.Content() = m_winxy;
		out_win.Write();
		Unlock ();
	}

	void DoRelease (void *)
	{
		m_som.Uninit ();
	}

	////////////////////////////

	// USE machine number to save filenames correctly.
	// build a som of NSTATES * M units to initialize the HMM.
	void InitializeBySOM (const CVisDVector **data, int nex, int T, CVisDVector **codes)
	{
		char buffer[256];
		memset (buffer, 0, 256);

		m_local_som.SetVerbosity(0);
		m_local_som.InitTrain1 ();

		printf ("Phase 1\n");
		int result = 0;
		for (int index = 0; result == 0; index++)
		{
			int n = YARPRandom::Rand (1, nex);
			int t = YARPRandom::Rand (1, T);
			result = m_local_som.Train1 ((CVisDVector&)data[n][t], 1, 0, 0);
			if ((index % 1000) == 0)
				printf ("step : %d\n", index);
		}

		m_local_som.EndTrain1 ();
		sprintf (buffer, "som_end1_%d.txt\0", m_machine);
		m_local_som.Serialize (buffer);

		m_local_som.InitTrain2 ();

		printf ("Phase 2\n");
		result = 0;
		for (index = 0; result == 0; index++)
		{
			int n = YARPRandom::Rand (1, nex);
			int t = YARPRandom::Rand (1, T);
			result = m_local_som.Train2 ((CVisDVector&)data[n][t], 1, 0, 0);
			if ((index % 1000) == 0)
				printf ("step : %d\n", index);
		}

		m_local_som.EndTrain2 ();
		sprintf (buffer, "som_end2_%d.txt\0", m_machine);
		m_local_som.Serialize (buffer);

		CVisDVector *tmp = new CVisDVector[NSTATES * M];
		assert (tmp != NULL);
		for (index = 0; index < NSTATES * M; index++)
			tmp[index].Resize (m_som_size);

		m_local_som.GetCodes (tmp);

		for (index = 0; index < NSTATES * M; index++)
		{
			codes[(index/M)+1][(index%M)+1] = tmp[index];
		}

		delete[] tmp;

		//m_local_som.SaveMap ("ex.cod");
		//m_local_som.SaveMapPlanes ("excel.cod");

		m_local_som.Uninit ();

		// save data to file.
		sprintf (buffer, "training_data_%d.txt\0", m_machine);
		FILE *fp = fopen (buffer, "w");
		assert (fp != NULL);

		for (int n = 1; n <= nex; n++)
			for (int t = 1; t <= T; t++)
			{
				SerializeVector (fp, data[n][t]);
			}

		fclose (fp);
	}

	// Hmm stuff.
	void AddNewHmm ()
	{
		Lock ();

		if (m_machine >= MAXOBJECTS)
		{
			printf ("AddNewHmm - memory exhausted\n");
			Unlock ();
			return;
		}

		//
		m_machine ++;
		m_hmm_objects[m_machine].Init (NSTATES, M, m_som_size, HmmDiag);

		printf ("Machine number %d added\n", m_machine);

		Unlock ();
	}

	// start getting observations.
	void StartCollectingDataHmm ()
	{
		Lock ();
		
		// reset the data collection procedure.
		m_nex = 1;
		m_T = 1;
		m_stage = SOM_CollectData; 

		Unlock ();
	}

	void TrainHmm ()
	{
		// it takes a while... this means the other thread waits!
		//Lock ();
		char buffer[256];
		memset (buffer, 0, 256);

		InitializeBySOM ((const CVisDVector **)m_observation, NEX, T, m_mu);
		m_hmm_objects[m_machine].NonRndKMeansInit ((const CVisDVector **)m_observation, NEX, T, (const CVisDVector **)m_mu);
		
		sprintf (buffer, "machine_before_%d.txt\0", m_machine);
		m_hmm_objects[m_machine].Serialize (buffer);

		CVisDVector LL;
		CVisDVector length (NEX);
		length = T;

		const int max_iter = 20;

		// Baum-Welch or/and Viterbi training.
		m_hmm_objects[m_machine].LearnViterbi ((const CVisDVector **)m_observation, length, NEX, LL, max_iter);
		m_hmm_objects[m_machine].Learn ((const CVisDVector **)m_observation, length, NEX, LL, 5);

		sprintf (buffer, "machine_%d.txt\0", m_machine);
		m_hmm_objects[m_machine].Serialize (buffer);

		//Unlock ();
	}

	void SerializeHmms()
	{
		if (m_machine < 0)
			return;

		char buffer[256];
		memset (buffer, 0, 256);

		for (int i = 0; i <= m_machine; i++)
		{
			sprintf (buffer, "machine_%d\0", m_machine);
			m_hmm_objects[i].Serialize (buffer);
		}
	}

	void UnserializeHmms(int max)
	{
		char buffer[256];
		memset (buffer, 0, 256);

		for (int i = 0; i < max; i++)
		{
			sprintf (buffer, "machine_%d\0", m_machine);
			m_hmm_objects[i].Unserialize (buffer);
		}

		m_machine = max - 1;
	}

	void LoadTrainDataHmm ()
	{
		char buffer[256];
		memset (buffer, 0, 256);

		sprintf (buffer, "training_data_%d.txt\0", m_machine);
		FILE *fp = fopen (buffer, "r");
		assert (fp != NULL);

		for (int n = 1; n <= NEX; n++)
			for (int t = 1; t <= T; t++)
			{
				UnserializeVector (fp, m_observation[n][t], m_som_size);
			}

		fclose (fp);
	}

	////////////////////////////

	void AddNewSample (CVisDVector& newsample)
	{
		Lock ();
		if (m_used_memory < memory_size)
		{
			m_last_entry = m_used_memory;
			m_training_set[m_used_memory] = newsample;
			m_used_memory ++;
		}
		else
		{
			// replace one at random
			int index = int((double(rand()) / RAND_MAX) * m_used_memory);
			if (index >= m_used_memory)
				index = m_used_memory-1;
			
			m_last_entry = index;
			m_new_good_sample = true;
			m_training_set[index] = newsample;
		}
		Unlock ();
	}

	void TestSample (CVisDVector& sample)
	{
		//
		Lock ();
		m_som.ComputeWinner (sample);
		m_som.GetWinner (m_winxy.x, m_winxy.y, m_winner);

		// wouldn't it be much better to send data from the main thread?
		// it doesn't really matter now. The vision thread communicates with
		// this.
		Unlock ();
	}

	int GetNumSamples (void)
	{
		Lock ();
		int ret = m_used_memory;
		Unlock ();
		return ret;
	}

	int GetNumSteps (void)
	{
		Lock ();
		int ret = m_steps;
		Unlock ();
		return ret;
	}

	// to move to end of learning stage.
	int StopSomTraining ()
	{
		Lock ();
		m_stage = SOM_Ended;
		Unlock ();

		return 0;
	}

	int Serialize (void)
	{
		Lock ();

		m_som.Serialize ("som_serialized.txt");

		Unlock ();

		return 0;
	}

	int Unserialize (void)
	{
		Lock ();

		int ret = m_som.Unserialize ("som_serialized.txt");
		if (ret == 2)
			m_stage = SOM_Two;
		else
			m_stage = SOM_One;

		m_steps = m_som.GetStep ();

		Unlock ();

		return ret;
	}

	int UnserializeAndLoad (void)
	{
		Lock ();

		int ret = m_som.Unserialize ("som_serialized.txt");
		if (ret == 2)
			m_stage = SOM_Two;
		else
			m_stage = SOM_One;

		FILE * fp = fopen ("som_training_set.txt","r");
		assert (fp != NULL);

		fscanf (fp, "%d\n", &m_used_memory);
		
		for (int i = 0; i < m_used_memory; i++)
		{
			double tmp;
			for (int j = 1; j <= m_som_size; j++)
			{
				fscanf (fp, "%lf ", &tmp);
				m_training_set[i](j) = tmp;
			}
			fscanf (fp, "\n");
		}

		fclose (fp);

		m_steps = m_som.GetStep ();

		Unlock ();

		return ret;
	}

	int SaveTrainingSet ()
	{
		Lock ();

		// save used_memory + all the samples.
		FILE * fp = fopen ("som_training_set.txt","w");
		assert (fp != NULL);

		fprintf (fp, "%d\n", m_used_memory);
		
		for (int i = 0; i < m_used_memory; i++)
		{
			for (int j = 1; j <= m_som_size; j++)
				fprintf (fp, "%lf ", m_training_set[i](j));
			fprintf (fp, "\n");
		}

		fclose (fp);

		Unlock ();
		return 0;
	}

	int LoadTrainingSet ()
	{
		Lock ();

		FILE * fp = fopen ("som_training_set.txt","r");
		assert (fp != NULL);

		fscanf (fp, "%d\n", &m_used_memory);
		
		for (int i = 0; i < m_used_memory; i++)
		{
			double tmp;
			for (int j = 1; j <= m_som_size; j++)
			{
				fscanf (fp, "%lf ", &tmp);
				m_training_set[i](j) = tmp;
			}
			fscanf (fp, "\n");
		}

		fclose (fp);
		Unlock ();
		return 0;
	}
};

static FILE *fp = NULL;
static int steps = 0;
//
// a few helper functions.
int DrawFixationPoint (YARPImageOf<YarpPixelRGB>& img, JointPos* pos, JointSpeed* speed)
{
	if (fp == NULL)
	{
		fp = fopen ("trajectory.txt","w");
		assert (fp != NULL);
	}

	unsigned char *a = (unsigned char *)img.GetAllocatedArray();
	const int size = img.GetWidth() * img.GetHeight() * 3;

	for (int i = 0; i < size; i++)
		if (*a > 1)
		{
			*a -= 2;
			a++;
		}
		else
			a++;

	double vergence = pos->j3 - pos->j2;
	double version = .5 * (pos->j3 + pos->j2);

	vergence /= 20000;
	if (vergence < 0) vergence = 0;
	if (vergence >= 1) vergence = 1-1e-3;

	int y = int(vergence * Size);

	version /= 30000;
	if (version < -.5) version = -.5;
	if (version >= .5) version = .5;

	int x = int(version * Size + Size/2);

	bool moving = false;
	moving |= (fabs(speed->j1) > 6000) ? true : false;
	moving |= (fabs(speed->j2) > 6000) ? true : false;
	moving |= (fabs(speed->j3) > 6000) ? true : false;
	moving |= (fabs(speed->j4) > 6000) ? true : false;
	moving |= (fabs(speed->j5) > 6000) ? true : false;
	moving |= (fabs(speed->j6) > 6000) ? true : false;
	moving |= (fabs(speed->j7) > 6000) ? true : false;

	if (fp != NULL)
	{
		fprintf (fp, "%lf %lf ", vergence, version);
	}

	if (vergence > .4 && vergence < 0.98 &&
		version < 0.2 && version > -0.2 &&
		!moving)
	{
		YarpPixelRGB black (0, 0, 0);
		img (x, y) = black;

		YarpPixelRGB red (255, 0, 0);
		img (x-1, y) = red;
		img (x+1, y) = red;
		img (x, y-1) = red;
		img (x, y+1) = red;
		if (fp != NULL)
		{
			fprintf (fp, "1\n");
			steps ++;
			if (steps > 10000)
			{
				fclose (fp);
				fp = NULL;
				steps = 0;
			}
		}
		return 1;
	}
	else
	if (vergence > .4 && vergence < 0.98 &&
		version < 0.2 && version > -0.2)
	{
		YarpPixelRGB green (0, 255, 0);
		img (x, y) = green;
		img (x-1, y) = green;
		img (x+1, y) = green;
		img (x, y-1) = green;
		img (x, y+1) = green;
	}
	else
	{
		YarpPixelRGB white (255, 255, 255);
		img (x, y) = white;
		if (fp != NULL)
		{
			fprintf (fp, "0\n");
			steps ++;
			if (steps > 10000)
			{
				fclose (fp);
				fp = NULL;
				steps = 0;
			}
		}
		return 0;
	}

	if (fp != NULL)
	{
		fprintf (fp, "0\n");
		steps ++;
		if (steps > 10000)
		{
			fclose (fp);
			fp = NULL;
			steps = 0;
		}
	}
	return 0;
}

int StaticFixationPoint (JointPos* pos, JointSpeed* speed)
{
	double vergence = pos->j3 - pos->j2;
	double version = .5 * (pos->j3 + pos->j2);

	// range 0-20000
	vergence /= 20000;
	if (vergence < 0) vergence = 0;
	if (vergence >= 1) vergence = 1-1e-3;

	// range -30000 +30000
	version /= 30000;
	if (version < -.5) version = -.5;
	if (version >= .5) version = .5;

	// this is not used.
	bool moving = false;
	moving |= (fabs(speed->j1) > 6000) ? true : false;
	moving |= (fabs(speed->j2) > 6000) ? true : false;
	moving |= (fabs(speed->j3) > 6000) ? true : false;
	moving |= (fabs(speed->j4) > 6000) ? true : false;
	moving |= (fabs(speed->j5) > 6000) ? true : false;
	moving |= (fabs(speed->j6) > 6000) ? true : false;
	moving |= (fabs(speed->j7) > 6000) ? true : false;

	if (vergence > .4 && vergence < 0.98 &&
		version < 0.2 && version > -0.2)
	{
		return 1;
	}
	else
	{
		return 0;
	}

	return 0;
}

// this calls the CSomThread...
class CFeatureThread : public Thread
{
protected:
	int hWidth, hHeight;
	int hZoom;

	// cartesian stuff.
	Image<PixelBGR> in_image;
	Image<PixelRGB> out_image;
	Image<PixelMono> out_image2;

	YARPImageOf<YarpPixelRGB> cartesian;
	YARPImageOf<YarpPixelRGB> logpolar;
	YARPImageOf<YarpPixelRGB> lp_out;

	YARPLpConverter lp;
	YARPClrHistogram histograms;

	const int fftmagic;
	YARPFftFeatures fft;

	JointSpeed j_speed;
	JointPos j_pos;

	YARPImageOf<YarpPixelMono> histo;
	YARPImageOf<YarpPixelRGB>  plot;

	YARPImageOf<YarpPixelMono> fftproc;
	YARPImageOf<YarpPixelMono> fftrec;

	CVisDVector tmpvect;
	CVisDVector streamlined;

	CSomThread * m_somthread;

public:
	CFeatureThread (int hwidth, int hheight) 
		: Thread (),
		  lp (nEcc, nAng, rfMin, Size),
		  histograms (nEcc, nAng, hwidth, hheight),
		  hZoom (5),
		  fft (nEcc, nAng),
		  fftmagic (FEATURESIZE)
	{
		hWidth = hwidth;
		hHeight = hheight;

		out_image.Create (Size, Size);
		out_image2.Create (Size, Size);

		cartesian.Resize (Size, Size);
		logpolar.Resize (nEcc, nAng);
		lp_out.Resize (nEcc, nAng);
	
		fftproc.Resize (nEcc, nAng);
		fftrec.Resize (Size, Size);

		in.SetName (in_img_name);
		in_speed.SetName (in_speed_name);
		in_pos.SetName (in_pos_name);

		out.SetName (out_img_name);
		out_h.SetName (out_histo_name);
	
		histo.Resize (hwidth+1, hheight+1);
		histo.Zero ();
		plot.Resize (Size, Size);
		plot.Zero ();

		tmpvect.Resize (hwidth + hheight + fftmagic);
		tmpvect = 0;
		streamlined.Resize (fftmagic);
		streamlined = 0;

		m_somthread = NULL;
	}

	virtual ~CFeatureThread () {}

	void SetSomPtr (CSomThread *p) { m_somthread = p; }
	void Start () { Thread::Begin (); }
	void Terminate () { Thread::End (); }

	virtual int Body ();
};

int CFeatureThread::Body ()
{
	while (1)
	{
		in.Read();
		in_image.Refer (in.Content());

		in_speed.Read();
		j_speed = in_speed.Content();
		in_pos.Read();
		j_pos = in_pos.Content();

		lp.Cart2LpAverageSwap (in_image, logpolar);

		histograms.Apply (logpolar);
		//histograms.ConvertHistogramToImage (histo);
		//YARPConverter::PasteInto (histo, 0, 0, hZoom, out_image2);

		CVisDVector& hrow = histograms.GetRowHistogram ();	// hHeigth
		CVisDVector& hcol = histograms.GetColHistogram ();	// hWidth

		for (int qq = 1; qq <= hHeight; qq++)
			tmpvect(qq) = hrow(qq);
		for (qq = 1; qq <= hWidth; qq++)
			tmpvect(qq+hHeight) = hcol(qq);

		if (m_somthread != NULL)
		{
			m_somthread->TestSample (tmpvect);
			if (DrawFixationPoint (plot, &j_pos, &j_speed))
			{
				// there's a target.

				// fft stuff.
				YARPColorConverter::RGB2Grayscale (logpolar, fftproc);
				fft.Apply (fftproc);
				fft.Reconstruct (fftproc);
				lp.Lp2Cart (fftproc, fftrec);
				YARPConverter::ConvertFromYARPToCog (fftrec, out_image2);

				// fill.
				fft.ZigZagScan (streamlined);
				for (qq = 1; qq <= fftmagic; qq++)
					tmpvect(qq+hWidth+hHeight) = streamlined(qq);

				cout << "there's a target" << endl;
				m_somthread->AddNewSample (tmpvect);
				cout << m_somthread->GetNumSamples() << " " << m_somthread->GetNumSteps() << endl;
			}
		}
		else
		{
			if (DrawFixationPoint (plot, &j_pos, &j_speed))
			{
				// there's a target.
				cout << "there's a target" << endl;
			}

		}

		YARPConverter::ConvertFromYARPToCog (plot, out_image);

		GenericImage& outImg = out.Content();
		outImg.Refer(out_image);
		out.Write();

		GenericImage& outImg2 = out_h.Content();
		outImg2.Refer(out_image2);
		out_h.Write();
	}
}



//
// globals.
CSomThread * somthread;
CFeatureThread * featurethread;


//
//
void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	const int hWidth = 15;
	const int hHeight = 10;

	const int MagicFft = FEATURESIZE;

	const int hZoom = 5;

	// initialize and run the Som thread.
	CVisDVector min(hWidth+hHeight+MagicFft);
	min = 0;
	CVisDVector max(hWidth+hHeight+MagicFft);
	
	max = 1400;
	for (int i = 1; i <= hWidth; i++)
	{
		max(hHeight+i) = 2000; //2000 / 2;
	}

	somthread = new CSomThread(hWidth+hHeight+MagicFft, min, max);
	assert (somthread != NULL);
	somthread->Start (1);

	// initialize and run the feature/img proc thread.
	featurethread = new CFeatureThread (hWidth, hHeight);
	assert (featurethread != NULL);

	// let the feature thread know about the som thread.
	featurethread->SetSomPtr (somthread);

	featurethread->Start ();

	// user interface here.
	char c;
	printf ("Threads started\n");
	printf ("som->> ");

	for (;;)
	{
		scanf ("%c", &c);

		switch (c)
		{
		case 'h':
			printf ("q : quit\n");
			printf ("s : save map to file\n");
			printf ("l : load map from file\n");
			printf ("t : stop som learning\n");
			printf ("-----------------------------------\n");
			printf ("a : add new HMM\n");
			printf ("d : load training data HMM\n");
			printf ("c : start collecting data new HMM\n");
			printf ("e : dump HMMs to file\n");
			printf ("b : unserialize HMMs from file\n");
			printf ("f : train latest HMM\n");
			printf ("som->> ");
			break;

		case 'a':
			somthread->AddNewHmm ();
			printf ("som->> ");
			break;

		case 'd':
			somthread->LoadTrainDataHmm ();
			printf ("som->> ");
			break;

		case 'c':
			somthread->StartCollectingDataHmm();
			printf ("som->> ");
			break;

		case 'e':
			somthread->SerializeHmms ();
			printf ("som->> ");
			break;

		case 'b':
			{
				int max = 0;
				scanf ("%d", &max);
				somthread->UnserializeHmms (max);
			}
			printf ("som->> ");
			break;

		case 'f':
			somthread->TrainHmm ();
			printf ("som->> ");
			break;

		case 't':
			somthread->StopSomTraining ();
			printf ("training of som mapped stopped\n");
			printf ("som->> ");
			break;

		case 's':
			somthread->Serialize ();
			somthread->SaveTrainingSet ();
			printf ("som->> ");
			break;

		case 'l':
			{
				int ret = somthread->UnserializeAndLoad ();
				if (ret == 2)
					printf ("...going to stage 2\n");
				else
					printf ("...going to stage 1\n");
				printf ("som->> ");
			}
			break;

		case 'q':
			{
				goto SmoothEnd;
			}
			break;
		}
	}

SmoothEnd:

	// quit.
	featurethread->Terminate ();
	somthread->Terminate (1);

	delete featurethread;
	delete somthread;
}
