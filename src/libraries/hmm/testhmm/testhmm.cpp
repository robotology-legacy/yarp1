//
//
//
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "YARPHmm.h"
#include "YARPImageServices.h"

#include "Som.h"

const int DIM = 16;
const int M = 10;
const int NSTATES = 5;

// image characteristics.
const int necc = 32;
const int nang = 64;
const double rf = 0.31;
const int size = 128;


//
//
//
void InitializeBySOM (const CVisDVector **data, int nex, int T, CVisDVector **codes)
{
	YARPSom som;
	
	CVisDVector omin(DIM);
	CVisDVector omax(DIM);

	omin = 0;
	omax = 1;

	som.Init (DIM, NSTATES, M,
		      10000, 0.05f, 10,
			  100000, 0.02f, 3,
			  omin, omax);	

	som.SetVerbosity(0);
	//som.SetSnapshotInfo ("pasa.dat", 1000);

	som.InitTrain1 ();

	printf ("Phase 1\n");
	int result = 0;
	for (int index = 0; result == 0; index++)
	{
		int n = YARPRandom::Rand (1, nex);
		int t = YARPRandom::Rand (1, T);
		result = som.Train1 ((CVisDVector&)data[n][t], 1, 0, 0);
		if ((index % 1000) == 0)
			printf ("step : %d\n", index);
	}

	som.EndTrain1 ();

	som.Serialize ("Som\\som_1.txt");

	som.InitTrain2 ();

	printf ("Phase 2\n");
	result = 0;
	for (index = 0; result == 0; index++)
	{
		int n = YARPRandom::Rand (1, nex);
		int t = YARPRandom::Rand (1, T);
		result = som.Train2 ((CVisDVector&)data[n][t], 1, 0, 0);
		if ((index % 1000) == 0)
			printf ("step : %d\n", index);
	}

	som.EndTrain2 ();

	som.Serialize ("Som\\som_2.txt");

	CVisDVector *tmp = new CVisDVector[NSTATES * M];
	assert (tmp != NULL);
	for (index = 0; index < NSTATES * M; index++)
		tmp[index].Resize (DIM);

	som.GetCodes (tmp);

	for (index = 0; index < NSTATES * M; index++)
	{
		codes[(index/M)+1][(index%M)+1] = tmp[index];
	}

	delete[] tmp;

	som.SaveMap ("Som\\ex.cod");
	som.SaveMapPlanes ("Som\\excel.cod");

	som.Uninit ();
}


//
//
void GenerateObservation (const YARPImageOf<YarpPixelRGB>& img, CVisDVector **o, int nex, int T)
{
	const int width = img.GetWidth ();
	const int height = img.GetHeight ();

	YARPImageOf<YarpPixelRGB> lp_rgb;
	YARPImageOf<YarpPixelMono> lp_mono;
	YARPImageOf<YarpPixelRGB> cropped;

	lp_rgb.Resize (necc, nang);
	lp_mono.Resize (necc, nang);
	cropped.Resize (size, size);

	YARPLogPolar lp (necc, nang, rf, size);
	YARPClrHistogram clr(necc, nang, 15, 10);
	YARPGaussianFeatures gauss(necc, nang, rf, size);

	CVisDVector tmp_features (DIM);
	CVisDVector color_features_1; // (10);
	CVisDVector color_features_2; // (15);
	CVisDVector ga_features; // (16);

	double gmax = -1e100;	// the max over the color histo.
	double xmax = -1e100;	// the max over the gaussian features.

	char filename[256];
	memset (filename, 0, 256);

	//YARPImageOf<YarpPixelHSV> hsv;
	//hsv.Resize (necc, nang);

	for (int n = 1; n <= nex; n++)
	{
		for (int t = 1; t <= T; t++)
		{
			// determine the fix point.
			// remove 1 more pixel just to be safe on the limit.
			int x = YARPRandom::Rand (size/2+1, width-size/2-1);
			int y = YARPRandom::Rand (size/2+1, height-size/2-1);

			// crop the image.
			((YARPImageOf<YarpPixelRGB>&)img).Crop (cropped, x-size/2, y-size/2);

			lp.Cart2LpAverage (cropped, lp_rgb);
			YARPColorConverter::RGB2Grayscale (lp_rgb, lp_mono);

			//lp.Lp2Cart (lp_rgb, cropped);
			//sprintf (filename, "OutImg\\cropped%d%d.ppm", n, t);
			//YARPImageFile::Write (filename, cropped);

			// color features.
			//YARPColorConverter::RGB2HSV (lp_rgb, hsv);
			//YARPImageUtils::GetHue (hsv, lp_mono);
			//YARPImageFile::Write ("OutImg\\hueimage.ppm", lp_mono);

			clr.Apply (lp_rgb);
			color_features_1 = clr.GetRowHistogram();
			color_features_2 = clr.GetColHistogram();

			// gaussian der features.
			gauss.Apply (lp_mono);
			ga_features = gauss.GetFeatures();

			// fill the feature vect.
			//for (int k = 1; k <= 9; k++)
			//	tmp_features(k) = color_features_1(k+1);
			//for (k = 1; k <= 15; k++)
			//	tmp_features(k+9) = color_features_2(k);
			//for (k = 1; k <= 16; k++)
			//	tmp_features(k+24) = ga_features(k);

			tmp_features = ga_features;

			//for (k = 1; k <= 24; k++)
			//	if (fabs(tmp_features(k)) > gmax)
			//		gmax = fabs(tmp_features(k));

			for (int k = 1; k <= DIM; k++)
				if (fabs(tmp_features(k)) > xmax)
					xmax = fabs(tmp_features(k));

			// save temp into obs.
			o[n][t] = tmp_features;

			//printf ("N : %d T : %d\n", n, t);
		}
		printf ("N : %d\n", n);
	}

	gmax /= 10;
	xmax /= 10;

	// normalization.
	for (n = 1; n <= nex; n++)
		for (int t = 1; t <= T; t++)
		{
			//for (int k = 1; k <= 24; k++)
			//	o[n][t](k) /= gmax;
			for (int k = 1; k <= DIM; k++)
				o[n][t](k) /= xmax;
		}

	// save obs file.
	FILE *fp = fopen (L"Hmm\\observation.txt", L"w");
	for (n = 1; n <= nex; n++)
		for (int t = 1; t <= T; t++)
		{
			SerializeVector (fp, o[n][t]);
		}
	fclose (fp);
}

void ReloadObservation (CVisDVector **o, int nex, int T)
{
	// save obs file.
	FILE *fp = fopen (L"Hmm\\observation.txt", L"r");
	for (int n = 1; n <= nex; n++)
		for (int t = 1; t <= T; t++)
		{
			UnserializeVector (fp, o[n][t], DIM);
		}
	fclose (fp);
}

//
//
//
void main ()
{
	YARPRandom::Seed (time(NULL));

	YARPMixtureHmm hmm;
	YARPMixtureHmm hmm2;

	YARPImageOf<YarpPixelRGB> image_in_cart;

	hmm.Init (NSTATES, M, DIM, HmmDiag);
	hmm2.Init (NSTATES, M, DIM, HmmDiag);

	// prepare the model.
	CVisDVector **mu = AllocVector (NSTATES, M, DIM);

	const int T = 100;
	const int nex = 30;

	CVisDVector **observation = AllocVector (nex, T, DIM);

	int choice = 0;
	printf ("generate (1) or reload (2) : ");
	scanf ("%d", &choice);

	if (choice == 1)
	{
		// generate observation from image - store observation in observation array.
		YARPImageFile::Read ("SourceImg\\Image2.ppm", image_in_cart);	
		GenerateObservation (image_in_cart, observation, nex, T);
	}
	else
		ReloadObservation (observation, nex, T);

	printf ("train HMM (1) or reload and test 2 HMM (2) : ");
	scanf ("%d", &choice);

	if (choice == 1)
	{
		// Use the SOM to initialize the HMM.
		InitializeBySOM ((const CVisDVector **)observation, nex, T, mu);
		hmm.NonRndKMeansInit ((const CVisDVector **)observation, nex, T, (const CVisDVector **)mu);

		//hmm.KMeansInit ((const CVisDVector **)observation, nex, T);
		hmm.Serialize ("Hmm\\test_2.txt");

		CVisDVector LL;
		CVisDVector length (nex);
		length = T;

		const int max_iter = 20;

		// Baum-Welch or/and Viterbi training.
//		hmm.LearnViterbi ((const CVisDVector **)observation, length, nex, LL, 1);
		hmm.Learn ((const CVisDVector **)observation, length, nex, LL, 10);
		hmm.Serialize ("Hmm\\learnt_2.txt");

		double like2 = hmm.ComputeLogLikelihood ((const CVisDVector **) observation, 1, T);
		printf ("like1 %lf\n", like2);
	}
	else
	{
		// data are reloaded.
		hmm.Unserialize ("Hmm\\learnt_2_image1.txt");
		//hmm2.Unserialize ("Hmm\\learnt_2_image2.txt");

//		for (int i = 0; i < 10; i++)
//		{
//			// test the first sequence.
//			double like1 = hmm.ComputeLogLikelihood ((const CVisDVector **)&observation[i], 1, T);
//			//double like2 = hmm2.ComputeLogLikelihood ((const CVisDVector **)&observation[i], 1, T);
//
//			//printf ("like1 %lf like2 %lf\n", like1, like2);
//			printf ("like1 %lf\n", like1);
//		}
	}

	// delete
	FreeVector (observation, nex);
	FreeVector (mu, NSTATES);
}


