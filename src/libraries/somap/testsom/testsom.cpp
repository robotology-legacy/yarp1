//
//
// very simple test of som lib.
#include <iostream.h>

#include <VisMatrix.h>

#include <Som.h>

void main(int argc, char *argv[])
{
	FILE *fp;

	fp = fopen ("ex.dat","r");
	assert (fp != NULL);

	int dim;
	fscanf (fp, "%d\n", &dim);

	CVisDVector sample(dim);
	sample = 0;

	YARPSom som;
	CVisDVector min(dim);
	CVisDVector max(dim);
	min(1) = -3;
	max(1) = 30;
	min(2) = 0;
	max(2) = 60;
	min(3) = -20;
	max(3) = 10;
	min(4) = -20;
	max(4) = 10;
	min(5) = 390;
	max(5) = 420;

	som.Init (dim, 12, 8,
		      10000, 0.05f, 10,
			  100000, 0.02f, 3,
			  min, max);	// just realized that a single min max is not adequate!

	som.SetVerbosity(0);
	som.SetSnapshotInfo ("pasa.dat", 1000);

	som.InitTrain1 ();

	int result = 0;
	int dummy;

	while (result == 0)
	{
		for (int i = 0; i < dim; i++)
		{
			float tmp;
			fscanf (fp, "%f ", &tmp);
			sample(i+1) = tmp;
		}
		fscanf(fp, "\n");

		if (feof (fp))
		{
			fseek (fp, 0, SEEK_SET);
			fscanf (fp, "%d\n", &dummy);
			assert (dummy == dim);
		}

		result = som.Train1 (sample, 1, 0, 0);
	}

	som.EndTrain1 ();

	som.Serialize ("test_serialize.txt");

	som.InitTrain2 ();

	result = 0;
	fseek (fp, 0, SEEK_SET);
	fscanf (fp, "%d\n", &dummy);
	assert (dummy == dim);

	while (result == 0)
	{
		for (int i = 0; i < dim; i++)
		{
			float tmp;
			fscanf (fp, "%f ", &tmp);
			sample(i+1) = tmp;
		}
		fscanf(fp, "\n");

		if (feof (fp))
		{
			fseek (fp, 0, SEEK_SET);
			fscanf (fp, "%d\n", &dummy);
			assert (dummy == dim);
		}

		result = som.Train2 (sample, 1, 0, 0);
	}

	som.EndTrain2 ();

	som.SaveMap ("ex.cod");
	som.SaveMapPlanes ("excel.cod");

	som.Uninit ();
}

