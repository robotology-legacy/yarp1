//
// Som.cpp
//

// wrapper class for the SOM library.
// there are still globals and statics in the lib... 
// just waiting for a bug to appear.
// Use only one instance of the class per process.
// Or be aware of the fact that there could be unexpected results.
// Most of the globals are actually parameters.

#include "Som.h"

int YARPSom::SetVerbosity (int level)
{
	return verbose (level);
}

YARPSom::YARPSom ()
{
	xdim = ydim = 0;

	dimension = 0;
	length1 = 0;
	alpha1 = 0;
	radius1 = 0;
	length2 = 0;
	alpha2 = 0;
	radius2 = 0;

	topol = TOPOL_HEXA;
	neigh = NEIGH_BUBBLE;

	data = NULL;
	codes = NULL;

	noc = 0;
	type_tmp = NULL;

	minrange = maxrange = NULL;

	initialized = false;

	memset (&params, 0, sizeof(struct teach_params));
}

YARPSom::~YARPSom ()
{
}

//
// dim: dimension of the in vectors.
// width: width of the map
// height: height of the map
// len1: length of the initial training
// a1: initial alpha value
// r1: initial neighborhood size
// len2: length of the second stage
// a2: initial alpha value 2nd stage
// r2: initial neighborhood size wnd stage
// min, max: component max and min value for initialization of the codebook
//

// NOTE: alpha and neighborhood size decrease to 0 and 1 respectively
// during training. Length is the estimated number of examples used.
// After Length steps learning is frozen... because of the values of
// alpha and radius.

int YARPSom::Init ( int dim,
				 int width, 
				 int height,
				 int len1,
				 float a1,
				 float r1,
				 int len2,
				 float a2,
				 float r2,
				 CVisDVector& min,
				 CVisDVector& max )
{
	// fixed. Modify here to use a different topology or neigh shape.
	topol = TOPOL_HEXA;
	neigh = NEIGH_BUBBLE;

	dimension = dim;
	xdim = width;
	ydim = height;

	length1 = len1;
	alpha1 = a1;
	radius1 = r1;

	length2 = len2;
	alpha2 = a2;
	radius2 = r2;

	minrange = new float[dim];
	maxrange = new float[dim];
	assert (minrange != NULL);
	assert (maxrange != NULL);

	for (int i = 0; i < dim; i++)
	{
		minrange[i] = min(i+1);
		maxrange[i] = max(i+1);
	}

	// if fixed or weights must be used change these lines to enable them.
	fixed = 0;		// no fixed points	
	weights = 0;	// no weights

	use_fixed(fixed);
	use_weights(weights);
	label_not_needed(1);

	// data are not read from file.
	data = alloc_entries ();
	assert (data != NULL);

	// intialize header.
	data->dimension = dimension;
	data->topol = topol;
	data->neigh = neigh;
	data->xdim = xdim;
	data->ydim = ydim;

	// not used perhaps.
    data->flags.loadmode = LOADMODE_ALL;

	struct data_entry_tag *entr = alloc_entry(data);
	data->entries = entr;
	data->num_loaded = 1;
	data->num_entries = 0;	// this is not known a-priori. use 0.
	data->lap = 0;
	data->fi = NULL;
	data->buffer = 0;		// see buffer.

	noc = xdim * ydim;
	assert (noc > 0);

    type_tmp = get_type_by_id(alpha_list, ALPHA_INVERSE_T); // ALPHA_LINEAR
	params.alpha_type = type_tmp->id;
	params.alpha_func = (ALPHA_FUNC *)type_tmp->data;

	// this is in params but is not used?.
	// int knn;                    /* nearest neighbours */

	init_random(0);	// use time to get a seed.
    codes = randinit_codes(data, topol, neigh, xdim, ydim, minrange, maxrange);
	assert (codes != NULL);

	// set general purpose params.
    set_teach_params(&params, codes, NULL, 0);
    set_som_params(&params);
    params.data = data;

	initialized = true;

	return 0;
}

// theoretically speaking. call again InitTrain1 to restart the training
// using the map in memory as starting point.
int YARPSom::InitTrain1 (void)
{
    params.length = length1;
    params.alpha = alpha1;
    params.radius = radius1;

	// start time.
	time(&params.start_time);
	data->lap = 0;		// internal time (in steps).

	return 0;
}

// to be done after intialization (before InitTrain1 perhaps).
int YARPSom::SetSnapshotInfo (char *filename, int interval)
{
	struct snapshot_info *info = new (struct snapshot_info);
	assert (info != NULL);
	info->filename = new char[1024];
	assert (info->filename != NULL);

	memset (info->filename, 0, 1024);
	memcpy (info->filename, filename, strlen(filename));

	info->interval = interval;
	info->type = SNAPSHOT_SAVEFILE;

	params.snapshot = info;

	return 0;
}

int YARPSom::ClearSnapshotInfo (void)
{
	if (params.snapshot != NULL)
	{
		if (params.snapshot->filename)
			delete[] params.snapshot->filename;
		delete params.snapshot;
	}

	params.snapshot = NULL;

	return 0;
}

int YARPSom::Train1 (CVisDVector& s, short wg, int fixx, int fixy)
{
	// need to prepare sample before executing this.
	// it must be done here.
	// remember to set both the fixed point and the weight.

	assert (s.Length() == data->dimension);
	
	struct data_entry_tag *sample;
	sample = data->entries;

	sample->next = NULL;
	if (use_weights(-1))
		sample->weight = wg;
	else
		sample->weight = 1;

	if (use_fixed(-1))
	{
		sample->fixed->xfix = fixx;
		sample->fixed->yfix = fixy;
	}
	else
		sample->fixed = NULL;

	sample->mask = NULL;	// no mask. Not used.
	sample->num_labs = 0;

	// fill the array.
	float *points = sample->points;
	double *sdata = s.data();
	for (int pp = 1; pp <= data->dimension; pp++)
		*points++ = (float)*sdata++;

	NEIGH_ADAPT *adapt;
	WINNER_FUNCTION *find_winner = params.winner;
	ALPHA_FUNC *get_alpha = params.alpha_func;

	int dim;
	int bxind, byind;
	float weight;
	float trad, talp;
	struct entries_tag *data = params.data;
	struct entries_tag *codes = params.codes;
	long length = params.length;
	float alpha = params.alpha;
	float radius = params.radius;
	struct snapshot_info *snap = params.snapshot;
	struct winner_info win_info;

	adapt = params.neigh_adapt;

	dim = codes->dimension;
	if (data->dimension != dim)
	{
		fprintf(stderr, "code dimension (%d) != data dimension (%d)\n",
				dim, data->dimension);
		return -1;
	}

	// sample has to be processed.
	weight = sample->weight;

	/* Radius decreases linearly to one */
	if (data->lap < length)
		trad = 1.0 + (radius - 1.0) * (float) (length - data->lap) / (float) length;
	else
		trad = 1.0;

	talp = get_alpha(data->lap, length, alpha);

	/* If the sample is weighted, we
	   modify the training rate so that we achieve the same effect as
	   repeating the sample 'weight' times */
	if ((weight > 0.0) && (use_weights(-1))) 
	{
		talp = 1.0 - (float) pow((double) (1.0 - talp), (double) weight);
	}

	/* Find the best match */
	/* If fixed point and is allowed then use that value */
	if ((sample->fixed != NULL) && (use_fixed(-1))) 
	{
		/* Get the values from fixed-structure */
		bxind = sample->fixed->xfix;
		byind = sample->fixed->yfix;
	}
	else 
	{
		if (find_winner(codes, sample, &win_info, 1) == 0)
		{
			ifverbose(3)
				fprintf(stderr, "ignoring empty sample %d\n", data->lap);
			goto skip_teach; /* ignore empty samples */
		}
		win_x = bxind = win_info.index % codes->xdim;
		win_y = byind = win_info.index / codes->xdim;
	}

	/* Adapt the units */
	adapt(&params, sample, bxind, byind, trad, talp);

skip_teach:
	/* save snapshot when needed */
	if ((snap) && ((data->lap % snap->interval) == 0) && (data->lap > 0))
	{
		ifverbose(2)
			fprintf(stderr, "Saving snapshot, %ld iterations\n", data->lap);

		if (save_snapshot(&params, data->lap))
		{
			fprintf(stderr, "snapshot failed, continuing teaching\n");
		}
	}

	ifverbose(1)
	  mprint((long) (length-data->lap));
	
	// this was used before to count the rounds.
	// it's now used to count the number of samples used.
	data->lap++; // how many times this function has been called.

	ifverbose(1)
	{
	  mprint((long) 0);
	  fprintf(stderr, "\n");
	}

	// warns in case steps expired... and radius == 1.
	return (trad == 1) ? -1 : 0;
}

// to explicitly compute the winner.
int YARPSom::ComputeWinner (CVisDVector& s)
{
	assert (s.Length() == data->dimension);
	
	struct data_entry_tag *sample;
	sample = data->entries;
	sample->next = NULL;
	sample->weight = 1;
	sample->fixed = NULL;
	sample->mask = NULL;	// no mask. Not used.
	sample->num_labs = 0;

	// fill the array.
	float *points = sample->points;
	double *sdata = s.data();
	for (int pp = 1; pp <= data->dimension; pp++)
		*points++ = (float)*sdata++;

	WINNER_FUNCTION *find_winner = params.winner;

	struct winner_info win_info;

	if (find_winner(codes, sample, &win_info, 1) == 0)
	{
		ifverbose(3)
			fprintf(stderr, "ignoring empty sample %d\n", data->lap);
		win_x = -1;
		win_y = -1;
	}
	else
	{
		win_x = win_info.index % codes->xdim;
		win_y = win_info.index / codes->xdim;
	}

	return 0;
}

int YARPSom::GetWinner (int& x, int& y, CVisDVector& result)
{
	x = win_x;
	y = win_y;

	int index = x + y * data->xdim;
	struct data_entry_tag *current = codes->entries;
	for (int i = 0; i < index; i++)
	{
		current = current->next;
	}

	for (i = 0; i < codes->dimension; i++)
		result(i+1) = current->points[i];

	return 0;
}

int YARPSom::GetSize (int& dimension, int& width, int& height)
{
	dimension = data->dimension;
	width = data->xdim;
	height = data->ydim;

	return 0;
}

// the array must be of size dim * width * height
int YARPSom::GetCodes (float *array)
{
	struct data_entry_tag *current = codes->entries;
	for (int i = 0; i < data->xdim * data->ydim; i++)
	{
		for (int j = 0; j < data->dimension; j++)
		{
			int index = i * data->dimension + j;
			array[index] = current->points[j];
		}
		current = current->next;
	}

	return 1;
}

int YARPSom::GetCodes (CVisDVector *array)
{
	struct data_entry_tag *current = codes->entries;
	for (int i = 0; i < data->xdim * data->ydim; i++)
	{
		for (int j = 0; j < data->dimension; j++)
		{
			array[i](j+1) = current->points[j];
		}
		current = current->next;
	}

	return 1;
}

int YARPSom::EndTrain1(void)
{
	time(&params.end_time);
	return 0;
}

// theoretically speaking. call again InitTrain2 to restart the training
// using the map in memory as starting point.
int YARPSom::InitTrain2 (void)
{    
    params.length = length2;
    params.alpha = alpha2;
    params.radius = radius2;

	// start time.
	time(&params.start_time);
	data->lap = 0;		// internal time (in steps).

	return 0;
}

int YARPSom::Train2 (CVisDVector& s, short weight, int fixx, int fixy)
{
	// warns if length2 steps are expired.
	return Train1 (s, weight, fixx, fixy);
}

int YARPSom::EndTrain2(void)
{
	time(&params.end_time);
	return 0;
}

int YARPSom::Uninit(void)
{
    close_entries(codes);
    codes = NULL;
	
	if (data)
		close_entries(data);
	data = NULL;

	if (minrange)
		delete[] minrange;
	minrange = NULL;
	if (maxrange)
		delete[] maxrange;
	maxrange = NULL;

	ClearSnapshotInfo ();

	initialized = false;

	return 0;
}

int YARPSom::SaveMap (char *filename)
{
    save_entries(codes, filename);

	return 0;
}

int YARPSom::Serialize (char *filename)
{
	FILE *fp;
	fp = fopen (filename, "w");
	assert (fp != NULL);

	fprintf (fp, "%d ", dimension);
	fprintf (fp, "%d ", xdim);
	fprintf (fp, "%d ", ydim);
	fprintf (fp, "%d ", topol);
	fprintf (fp, "%d\n", neigh);
	fprintf (fp, "%f ", alpha1);
	fprintf (fp, "%f ", radius1);
	fprintf (fp, "%d ", length1);
	fprintf (fp, "%f ", alpha2);
	fprintf (fp, "%f ", radius2);
	fprintf (fp, "%d ", length2);
	fprintf (fp, "%d ", fixed);
	fprintf (fp, "%d\n", weights);

	for (int kk = 0; kk < dimension; kk++)
		fprintf (fp, "%f %f ", minrange[kk], maxrange[kk]);
	fprintf (fp, "\n");

	// write data params.	
	fprintf (fp, "%d ", data->flags.loadmode);
	fprintf (fp, "%d ", data->flags.labels_needed);
	fprintf (fp, "%d ", data->flags.random_order);
	fprintf (fp, "%d ", data->flags.skip_empty);
	fprintf (fp, "%d\n", data->flags.totlen_known);

	fprintf (fp, "%d ", data->lap);
	fprintf (fp, "%d\n", data->buffer);

	struct data_entry_tag *current = codes->entries;

	// write codes. The params in codes can be reconstructed from
	// those in data.
	int codebooksize = xdim * ydim;
	for (int i = 0; i < codebooksize; i++)
	{
		// write vector.
		for (int j = 0; j < codes->dimension; j++)
			fprintf (fp, "%f ", current->points[j]);
		fprintf (fp, "\n");

		if (current->mask != NULL)
		{
			for (j = 0; j < codes->dimension; j++)
				fprintf (fp, "%d ", current->mask[j]);
			fprintf (fp, "\n");
		}

		// fixpoint
		if ((current->fixed != NULL) && (use_fixed(-1))) 
		{
			/* Get the values from fixed-structure */
			fprintf (fp, "%d %d\n", current->fixed->xfix, current->fixed->yfix);
		}
		else
			fprintf (fp, "0 0\n");

#if 0
		// LATER: implement save/load of labels in serialize/unserialize.

		// label
		int label;
	    for (j = 0;;j++)
		{
			label = get_entry_labels(current, j);
			if (label != LABEL_EMPTY) 
				fprintf(fp, "%s ", find_conv_to_lab(label));
			else
				break;
		}
		fprintf(fp, "\n");
#endif

		// jump to the next entry
		current = current->next;
	}

	// params... save only current radius, alpha, params.
	fprintf (fp, "%f ", params.radius);
	fprintf (fp, "%f ", params.alpha);
	fprintf (fp, "%d\n", params.length);

	fclose (fp);
	return 0;
}

int YARPSom::SaveMapPlanes (char *filename)
{
	FILE *fp;
	fp = fopen (filename, "w");
	assert (fp != NULL);

	// write codes. The params in codes can be reconstructed from
	// those in data.

	for (int k = 0; k < dimension; k++)
	{	
		struct data_entry_tag *current = codes->entries;
		for (int i = 0; i < ydim; i++)
		{
			for (int j = 0; j < xdim; j++)
			{
				fprintf (fp, "%f ", current->points[k]);
				current = current->next;
			}

			fprintf (fp, "\n");
		}

		fprintf (fp, "\n\n");
	}

	fclose (fp);
	return 0;
}

int YARPSom::Unserialize (char *filename)
{
	FILE *fp;
	fp = fopen (filename, "r");
	assert (fp != NULL);

	//
	if (initialized)
		Uninit ();

	fscanf (fp, "%d ", &dimension);
	fscanf (fp, "%d ", &xdim);
	fscanf (fp, "%d ", &ydim);
	fscanf (fp, "%d ", &topol);
	fscanf (fp, "%d\n", &neigh);
	fscanf (fp, "%f ", &alpha1);
	fscanf (fp, "%f ", &radius1);
	fscanf (fp, "%d ", &length1);
	fscanf (fp, "%f ", &alpha2);
	fscanf (fp, "%f ", &radius2);
	fscanf (fp, "%d ", &length2);
	fscanf (fp, "%d ", &fixed);
	fscanf (fp, "%d\n", &weights);

	CVisDVector min, max;
	min.Resize (dimension);
	max.Resize (dimension);

	double tmin, tmax;

	for (int kk = 0; kk < dimension; kk++)
	{
		fscanf (fp, "%f %f ", &tmin, &tmax);
		min(kk+1) = tmin;
		max(kk+1) = tmax;
	}
	fscanf (fp, "\n");
	
	// intialize...
	Init (dimension, xdim, ydim,
		  length1, alpha1, radius1, length2, alpha2, radius2,
		  min, max);

	// read data params.
	int dummy;
	fscanf (fp, "%d ", &dummy);
	fscanf (fp, "%d ", &dummy);
	fscanf (fp, "%d ", &dummy);
	fscanf (fp, "%d ", &dummy);
	fscanf (fp, "%d\n", &dummy);

	fscanf (fp, "%d ", &data->lap);
	fscanf (fp, "%d\n", &dummy);

	struct data_entry_tag *current = codes->entries;

	// read codes. The params in codes can be reconstructed from
	// those in data.
	int codebooksize = xdim * ydim;
	for (int i = 0; i < codebooksize; i++)
	{
		// read vector.
		for (int j = 0; j < codes->dimension; j++)
			fscanf (fp, "%f ", &(current->points[j]));
		fscanf (fp, "\n");

		if (current->mask != NULL)
		{
			for (j = 0; j < codes->dimension; j++)
				fscanf (fp, "%d ", &(current->mask[j]));
			fscanf (fp, "\n");
		}

		// fixpoint (must be already allocated).
		if (current->fixed != NULL && use_fixed(-1))
			fscanf (fp, "%d %d\n", &current->fixed->xfix, &current->fixed->yfix);
		else
			fscanf (fp, "%d %d\n", &dummy, &dummy);

		// LATER: implement label stuff. See also Serialize.
#if 0
		// label
	    for (j = 0;;j++)
		{
			char c;
			fread (&c, 1, 1, fp);
			if (c == '\n')
				break;
		}
#endif

		// jump to the next entry
		current = current->next;
	}

	// params... read only current radius, alpha, params.
	fscanf (fp, "%f ", &params.radius);
	fscanf (fp, "%f ", &params.alpha);
	fscanf (fp, "%d\n", &params.length);

	int stage = 1;
	if (params.radius == radius1 && params.alpha == alpha1 && params.length == length1)
	{
		// stage 1.
		stage = 1;
	}
	else
	{
		// stage 2.
		stage = 2;
	}

//	printf ("%f %f %f\n", params.radius, radius1, radius2);

	fclose (fp);
	return stage;
}