// LookupTable.cpp : Implementation of CLookupTable
#include "LookupTable.h"

///////////////////////////////////////////////////////////////////////////////
// implementation of the __CGTable class.
//
//	Questa classe e' una tabella N dimensionale con possibilita' di accesso 
//	nearest-neighbor. La tabella e' inizialmente vuota, le varie unita' vengono
//	aggiunte quando servono.
//

///////////////////////////////////////////////////////////////////////////////
//
// class __CTableUnit.
//
//
__CTableUnit::__CTableUnit(void)
{
	nelem = 0;
}

__CTableUnit::__CTableUnit(const __CTableUnit& u)
{
	nelem = u.nelem;
	x = u.x;
	y = u.y;
}

///////////////////////////////////////////////////////////////////////////////
//
// classe __CGTable.
//
//
__CGTable::__CGTable(void)
{
	dimx = dimy = 0;
	threshold = 0;
	eta = 0;
	n_steps = 0;
}

__CGTable::__CGTable(int dx, int dy, double th, double e)
{
	dimx = dx;
	dimy = dy;
	eta = e;
	zero.Resize(dimy);
	zero = 0;
	lasterror = 0;
	threshold = th * th;

	v_temp.Resize(dimx);
	vv_temp.Resize(dimx);
	n_steps = 0;
}

void __CGTable::Rebuild(int dx, int dy, double th, double e)
{
	dimx = dx;
	dimy = dy;
	eta = e;
	zero.Resize(dimy);
	zero = 0;
	lasterror = 0;
	threshold = th * th;

	v_temp.Resize(dimx);
	vv_temp.Resize(dimx);

	erase(begin(), end()); // cancella la lista.
	n_steps = 0;
}

__CGTable::~__CGTable(void) 
{
}

CVisDVector __CGTable::Nearest (const CVisDVector& x)
{
	assert (x.Length() == dimx && dimx != 0);

	if (empty()) 
	{
		lasterror = threshold + 1;	// bisogna aggiungere di sicuro una
									// nuova unita'.
		return zero;
	}
	else
	{
		// cerca l'elemento a distanza minima da x.
		POSITION index, i_tmp;
		index = begin();
		lasterror = -1.0; // valore iniziale.

		while (index != end())
		{
			i_tmp = index;
			v_temp = (*index).getx();
			index++;

			if (lasterror == -1.0)
			{
				imin = i_tmp;
				vv_temp = v_temp - x;
				lasterror = vv_temp.norm2square();	
			}
			else
			{
				vv_temp = v_temp - x;
				double error = vv_temp.norm2square();	
				if (error < lasterror)
				{
					imin = i_tmp;
					lasterror = error;
				}
			}
		}

		return (*imin).gety();
	}
}

//
// MODIFICATO: 23/11/98.
//
void __CGTable::CheckAndAdd (const CVisDVector& x, const CVisDVector& y)
{
	// controllo se sono sopra la soglia.
	if (lasterror > threshold)
	{
		// aggiungo una nuova unita'.
		s_temp.reset ();
		s_temp.add (x, y, 0.0);
		push_back (s_temp);
	}
	else
	{
		// aggiungo solo il nuovo dato.
		(*imin).add (x, y, eta);
	}
	n_steps ++;
}

void __CGTable::AddDelta (const CVisDVector& x, const CVisDVector& delta)
{
	CVisDVector y;
	y = Nearest (x);

	// controllo se sono sopra la soglia.
	if (lasterror > threshold)
	{
		// aggiungo una nuova unita'.
		s_temp.reset ();
		s_temp.add_delta (x, y + delta);
		push_back (s_temp);
	}
	else
	{
		// aggiungo solo il nuovo dato.
		(*imin).add_delta (x, delta);
	}

	n_steps ++;
}

// Add a new unit, without any check.
void __CGTable::AddNew (const CVisDVector& x, const CVisDVector& y)
{
	s_temp.reset ();
	s_temp.add (x, y, 0.0);
	push_back (s_temp);

	n_steps ++;
}

// returning -1 means that the operation failed.
int __CGTable::GetRandom (CVisDVector& x, CVisDVector& y)
{
	if (empty ())
	{
		x = 0;
		y = 0;
		return -1;
	}

	// rnd value.
	const int last = size ();
	int value = int(double(rand ()) * last / double(RAND_MAX) + 1);

	POSITION pos = begin() + value;
	if (pos == NULL)
	{
		x = 0;
		y = 0;
		return -1;
	}

	x = (*pos).getx ();
	y = (*pos).gety ();

	return 0;
}

//
// ASCII dump on file.
//
bool __CGTable::DumpToFile (const _TCHAR *filename) const
{
	assert (filename != NULL);
	if (empty())
		return false;

	FILE *fp = NULL;
	fp = fopen (filename, _T("w"));
	if (fp == NULL)
		return false;

	fprintf (fp, _T("%d %d %d\n"), size (), dimx, dimy);
	fprintf (fp, _T("%d %lf\n"), GetNumberOfSamples (), sqrt(threshold));

	// search max/min per il display.
	POSITION index;
	__CTableUnit u_tmp;
	int i;

	index = (POSITION)begin();

	CVisDVector minx(dimx);
	CVisDVector maxx(dimx);
	
	u_tmp = (*index);
	minx = u_tmp.getx();
	maxx = u_tmp.getx();

	while (index != end())
	{
		u_tmp = (*index);
		index ++;

		for (i = 1; i <= dimx; i++)
		{
			double temp = u_tmp.getx()(i);
			if (temp > maxx(i)) maxx(i) = temp;
			if (temp < minx(i)) minx(i) = temp;
		}
	}

	for (i = 1; i <= dimx; i++)	
		fprintf (fp, _T("%lf "), minx(i));
	fprintf (fp, _T("\n"));

	for (i = 1; i <= dimx; i++)	
		fprintf (fp, _T("%lf "), maxx(i));
	fprintf (fp, _T("\n"));

	index = (POSITION)begin();

	while (index != end())
	{
		u_tmp = (*index);
		index ++;

		for (i = 1; i <= dimx; i++)
			fprintf (fp, _T("%lf "), u_tmp.getx()(i));
		
		for (i = 1; i <= dimy; i++)
			fprintf (fp, _T("%lf "), u_tmp.gety()(i));

		fprintf (fp, _T("%d "), u_tmp.getnelem());
		fprintf (fp, _T("%lf "), 0.0);
		fprintf (fp, _T("\n"));
	}

	fclose (fp);

	return true;
}

//
// Carico solo i dati .. l'izializzazione deve essere mantenuta (thresh, ecc).
//
// LATER: load also threshold, and number of samples.
//
bool __CGTable::LoadFromFile (const _TCHAR *filename)
{
	assert (filename != NULL);

	// free the list.
	erase(begin(), end());

	int nItems = 0;
	int tdimx, tdimy;

	FILE *fp = NULL;
	fp = fopen (filename, _T("r"));
	if (fp == NULL)
		return false;

	fscanf (fp, _T("%d%d%d"), &nItems, &tdimx, &tdimy);
	assert (tdimx == dimx && tdimy == dimy);

	double tmp;
	fscanf (fp, _T("%d %lf\n"), &n_steps, &tmp);
	ChangeResolution (tmp);

	// read out min and max.
	for (int i = 1; i <= 2 * dimx; i++)
	{
		fscanf (fp, _T("%lf"), &tmp);
	}

	__CTableUnit u_tmp;
	u_tmp.reset();
	CVisDVector x(dimx);
	CVisDVector y(dimy);
	int nelem;
	double cost;

	for (i = 1; i <= nItems; i++)
	{
		for (int j = 1; j <= dimx; j++)
		{
			fscanf (fp, _T("%lf"), &tmp);
			x(j) = tmp; 
		}

		u_tmp.getx() = x;

		for (j = 1; j <= dimy; j++)
		{
			fscanf (fp, _T("%lf"), &tmp);
			y(j) = tmp;
		}

		u_tmp.gety() = y;

		fscanf (fp, _T("%d"), &nelem);
		u_tmp.getnelem() = nelem;

		fscanf (fp, _T("%lf\n"), &cost);
		//u_tmp.getcost() = 0;

		push_back(u_tmp);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CLookupTable


XX_STDMETHODIMP CLookupTable::Init(int n, int m, double res)
{
	m_map.Rebuild (n, m, res, 0.0);
	m_tmp_x.Resize (n);
	m_tmp_y.Resize (m);

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::Nearest(int size, double *v_in, int size2, double *v_out)
{
	memcpy (m_tmp_x.data(), v_in, size * sizeof(double));
	m_tmp_y = m_map.Nearest (m_tmp_x);
	memcpy (v_out, m_tmp_y.data(), size2 * sizeof(double));

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::CheckAndAdd(int size, double *v_in, int size2, double *v_out)
{
	memcpy (m_tmp_x.data(), v_in, size * sizeof(double));
	memcpy (m_tmp_y.data(), v_out, size2 * sizeof(double));
	m_map.CheckAndAdd (m_tmp_x, m_tmp_y);

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::LoadFromFile(int size, _TCHAR *filename)
{
	m_map.LoadFromFile (filename);

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::DumpToFile(int size, _TCHAR *filename)
{
	m_map.DumpToFile (filename);

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::GetRandom(int size, double *v_in, int size2, double *v_out)
{
	m_map.GetRandom (m_tmp_x, m_tmp_y);
	memcpy (v_in, m_tmp_x.data(), size * sizeof(double));
	memcpy (v_out, m_tmp_y.data(), size2 * sizeof(double));

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::AddNew(int size, double *v_in, int size2, double *v_out)
{
	memcpy (m_tmp_x.data(), v_in, size * sizeof(double));
	memcpy (m_tmp_y.data(), v_out, size2 * sizeof(double));
	m_map.AddNew (m_tmp_x, m_tmp_y);

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::AddDelta(int size, double *v_in, int size2, double *v_out)
{
	memcpy (m_tmp_x.data(), v_in, size * sizeof(double));
	memcpy (m_tmp_y.data(), v_out, size2 * sizeof(double));
	m_map.AddDelta (m_tmp_x, m_tmp_y);

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::GetLastError(double *error)
{
	*error = m_map.LastError ();

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::ChangeResolution(double newres)
{
	m_map.ChangeResolution (newres);

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::GetNumberOfSamples(int *samples)
{
	*samples = m_map.GetNumberOfSamples ();

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::GetResolution(double *res)
{
	*res = m_map.GetResolution ();

	return XX_S_OK;
}

XX_STDMETHODIMP CLookupTable::Rebuild(int dx, int dy, double thr)
{
	m_map.Rebuild (dx, dy, thr, 0.0);

	return XX_S_OK;
}
