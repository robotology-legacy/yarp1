// StaticLut.h: interface for the CStaticLut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATICLUT_H__65AA4E11_D319_11D2_A4BD_0060087AA09D__INCLUDED_)
#define AFX_STATICLUT_H__65AA4E11_D319_11D2_A4BD_0060087AA09D__INCLUDED_

#include <assert.h>

// OS specific.

#ifdef WIN32

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _ASSERT
#	define _ASSERT(xx) assert(xx)
#	define _DEFINED_HERE 1
#endif

#endif

#ifdef __QNX__
#include <YARPSafeNew.h>
#define _ASSERT(xx) assert(xx)
#endif

#ifdef __LINUX__
#define _ASSERT(xx) assert(xx)
#endif

// CStaticLut 2D Only.
template <class T> class CStaticLut  
{
private:
	double lowx, hix;
	double lowy, hiy;
	int sizex, sizey;
	double howx, howy;
	double cx, cy;
	T max;

	T **array;

	CStaticLut(const CStaticLut<T>&);
	double dropped;
	double dropped_std;

public:
	CStaticLut(void);
	CStaticLut(double lx, double hx, double ly, double hy, int nx, int ny);
	void Resize(double lx, double hx, double ly, double hy, int nx, int ny);
	virtual ~CStaticLut(void);

	void operator= (const CStaticLut<T>&);
	void operator=(T ini);
	void operator/=(T val);
	T& operator()(double x, double y);
	T& operator()(int i, int j);

	inline double GetX(int i) const;
	inline double GetY(int j) const;
	inline int GetIndexX(double x) const;
	inline int GetIndexY(double y) const;
	inline int GetWidth(void) const { return sizex; }
	inline int GetHeight(void) const { return sizey; }
	inline T** GetArrayPtr(void) { return array; }
	inline T& GetMaximum(void) { return max; }

	inline double& GetDropped(void) { return dropped; }
	inline double& GetDroppedStd(void) { return dropped_std; }
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T> void CStaticLut<T>::operator= (const CStaticLut<T>& src)
{
	if (this == &src)
		return;

	if (src.sizex != sizex || src.sizey != sizey)
	{
		if (array != NULL)
		{
			delete[] array[0];
			delete[] array;
		}

		sizex = src.sizex;
		sizey = src.sizey;

		// alloc.
		array = new T*[sizey];
		_ASSERT (array != NULL);

		array[0] = new T [sizex * sizey];
		_ASSERT (array[0] != NULL);

		for (int i = 1; i < sizey; i++)
		{
			array[i] = array[0] + sizex * i;	
		}
	}
	else
	{
		sizex = src.sizex;
		sizey = src.sizey;
	}

	// data.
	memcpy (array[0], src.array[0], sizeof(T) * sizex * sizey);

	lowx = src.lowx;
	lowy = src.lowy;
	hix = src.hix;
	hiy = src.hiy;
	howx = src.howx;
	howy = src.howy;
	cx = src.cx;
	cy = src.cy;

	max = src.max;
	dropped = src.dropped;
	dropped_std = src.dropped_std;
}

template <class T> CStaticLut<T>::CStaticLut(void)
{
	lowx = hix = 0;
	lowy = hiy = 0;
	sizex = sizey = 0;
	howx = howy = 0;
	cx = cy = 0;
	dropped = 0;
	dropped_std = 0;

	array = NULL;
	max = 0;
}

template <class T> CStaticLut<T>::CStaticLut (
						double lx, 
						double hx, 
						double ly, 
						double hy,
						int nx,
						int ny)
{
	lowx = lx;
	lowy = ly;
	hix = hx;
	hiy = hy;
	sizex = nx;
	sizey = ny;

	// 1 cell added to cope with fractional intervals.
	howx = int ((hx - lx) / double(sizex) + 1);
	howy = int ((hy - ly) / double(sizey) + 1);

	cx = (sizex - 1) / (hx - lx);
	cy = (sizey - 1) / (hy - ly);

	// alloc.
	array = new T*[sizey];
	_ASSERT (array != NULL);

	array[0] = new T [sizex * sizey];
	_ASSERT (array[0] != NULL);

	for (int i = 1; i < sizey; i++)
	{
		array[i] = array[0] + sizex * i;	
	}

	max = 0;
	dropped = 0;
	dropped_std = 0;
}

template <class T> void CStaticLut<T>::Resize ( double lx, 
												double hx, 
												double ly, 
												double hy,
												int nx,
												int ny)
{
	if (array != NULL)
	{
		delete[] array[0];
		delete[] array;
		array = NULL;
	}

	lowx = lx;
	lowy = ly;
	hix = hx;
	hiy = hy;
	sizex = nx;
	sizey = ny;

	// 1 cell added to cope with fractional intervals.
	howx = int ((hx - lx) / double(sizex) + 1);
	howy = int ((hy - ly) / double(sizey) + 1);

	cx = (sizex - 1) / (hx - lx);
	cy = (sizey - 1) / (hy - ly);

	// alloc.
	array = new T*[sizey];
	_ASSERT (array != NULL);

	array[0] = new T [sizex * sizey];
	_ASSERT (array[0] != NULL);

	for (int i = 1; i < sizey; i++)
	{
		array[i] = array[0] + sizex * i;	
	}

	max = 0;
	dropped = 0;
	dropped_std = 0;
}

template <class T> CStaticLut<T>::~CStaticLut(void)
{
	if (array != NULL)
	{
		delete[] array[0];
		delete[] array;
	}
}

template <class T> void CStaticLut<T>::operator=(T ini)
{
	const int size = sizex * sizey;
	T *tmp = array[0];
	for (int i = 0; i < size; i++)
	{
		*tmp++ = ini;
	}

	max = ini;
	dropped = 0;
	dropped_std = 0;
}

template <class T> void CStaticLut<T>::operator/=(T val)
{
	const int size = sizex * sizey;
	T *tmp = array[0];
	for (int i = 0; i < size; i++)
	{
		*tmp++ /= val;
	}

	max /= val;
}

template <class T> T& CStaticLut<T>::operator()(double x, double y)
{
	// convert index.
	int xx = 0, yy = 0;

	xx = int ((x - lowx) * cx + .5);
	yy = int ((y - lowy) * cy + .5);
	_ASSERT (xx >= 0 && xx < sizex && yy >= 0 && yy < sizey);

	return array[yy][xx];
}

template <class T> T& CStaticLut<T>::operator()(int i, int j)
{
	_ASSERT (i >= 0 && i < sizex && j >= 0 && j < sizey);
	return array[j][i];
}

template <class T> inline double CStaticLut<T>::GetX(int i) const
{
	_ASSERT (i >= 0 && i < sizex);
	return i / cx + lowx;
}

template <class T> inline double CStaticLut<T>::GetY(int j) const
{
	_ASSERT (j >= 0 && j < sizey);
	return j / cy + lowy;
}

template <class T> inline int CStaticLut<T>::GetIndexX(double x) const
{
	_ASSERT (x >= lowx && x <= hix);
	return int ((x - lowx) * cx + .5);
}

template <class T> inline int CStaticLut<T>::GetIndexY(double y) const
{
	_ASSERT (y >= lowy && y <= hiy);
	return int ((y - lowy) * cy + .5);
}


//
//
//
//
//
//

// CStaticLut 3D Only.
template <class T> class CStaticLut3D  
{
private:
	double lowx, hix;
	double lowy, hiy;
	double lowz, hiz;
	int sizex, sizey, sizez;
	double howx, howy, howz;
	double cx, cy, cz;
	T max;

	T ***array;

	CStaticLut3D(const CStaticLut3D<T>&);
	double dropped;
	double dropped_std;

	void _alloc(void)
	{
		array = new T**[sizex];
		_ASSERT (array != NULL);
		array[0] = new T*[sizex * sizey];
		_ASSERT (array[0] != NULL);
		array[0][0] = new T [sizex * sizey * sizez];
		_ASSERT (array[0][0] != NULL);

		for (int i = 1; i < sizex; i++)
		{
			array[i] = array[0] + sizey * i;
		}

		for (i = 0; i < sizex; i++)
			for (int j = 0; j < sizey; j++)
			{
				array[i][j] = array[0][0] + sizez * j + sizez * sizey * i;
			}
	}

public:
	CStaticLut3D(void);
	CStaticLut3D(double lx, double hx, double ly, double hy, double lz, double hz, int nx, int ny, int nz);
	void Resize(double lx, double hx, double ly, double hy, double lz, double hz, int nx, int ny, int nz);
	virtual ~CStaticLut3D(void);

	void operator= (const CStaticLut3D<T>&);
	void operator=(T ini);
	void operator/=(T val);
	T& operator()(double x, double y, double z);
	T& operator()(int i, int j, int k);

	inline double GetX(int i) const;
	inline double GetY(int j) const;
	inline double GetZ(int k) const;
	inline int GetIndexX(double x) const;
	inline int GetIndexY(double y) const;
	inline int GetIndexZ(double z) const;
	inline int GetWidth(void) const { return sizex; }
	inline int GetHeight(void) const { return sizey; }
	inline int GetDepth(void) const { return sizez; }
	inline T*** GetArrayPtr(void) { return array; }
	inline T& GetMaximum(void) { return max; }

	inline double& GetDropped(void) { return dropped; }
	inline double& GetDroppedStd(void) { return dropped_std; }
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T> void CStaticLut3D<T>::operator= (const CStaticLut3D<T>& src)
{
	if (this == &src)
		return;

	if (src.sizex != sizex || src.sizey != sizey || src.sizez != sizez)
	{
		if (array != NULL)
		{
			delete[] array[0][0];
			delete[] array[0];
			delete[] array;
		}

		sizex = src.sizex;
		sizey = src.sizey;

		_alloc();	
	}
	else
	{
		sizex = src.sizex;
		sizey = src.sizey;
		sizez = src.sizez;
	}

	// data.
	memcpy (array[0][0], src.array[0][0], sizeof(T) * sizex * sizey * sizez);

	lowx = src.lowx;
	lowy = src.lowy;
	lowz = src.lowz;
	hix = src.hix;
	hiy = src.hiy;
	hiz = src.hiz;
	howx = src.howx;
	howy = src.howy;
	howz = src.howz;
	cx = src.cx;
	cy = src.cy;
	cz = src.cz;

	max = src.max;
	dropped = src.dropped;
	dropped_std = src.dropped_std;
}

template <class T> CStaticLut3D<T>::CStaticLut3D(void)
{
	lowx = hix = 0;
	lowy = hiy = 0;
	lowz = hiz = 0;
	sizex = sizey = sizez = 0;
	howx = howy = howz = 0;
	cx = cy = cz = 0;
	dropped = 0;
	dropped_std = 0;

	array = NULL;
	max = 0;
}

template <class T> CStaticLut3D<T>::CStaticLut3D (
						double lx, 
						double hx, 
						double ly, 
						double hy,
						double lz, 
						double hz,
						int nx,
						int ny,
						int nz)
{
	lowx = lx;
	lowy = ly;
	lowz = lz;
	hix = hx;
	hiy = hy;
	hiz = hz;
	sizex = nx;
	sizey = ny;
	sizez = nz;

	// 1 cell added to cope with fractional intervals.
	howx = int ((hx - lx) / double(sizex) + 1);
	howy = int ((hy - ly) / double(sizey) + 1);
	howz = int ((hz - lz) / double(sizez) + 1);

	cx = (sizex - 1) / (hx - lx);
	cy = (sizey - 1) / (hy - ly);
	cz = (sizez - 1) / (hz - lz);

	_alloc ();

	max = 0;
	dropped = 0;
	dropped_std = 0;
}

template <class T> void CStaticLut3D<T>::Resize ( double lx, 
												  double hx, 
												  double ly, 
												  double hy,
												  double lz, 
												  double hz,
												  int nx,
												  int ny,
												  int nz)
{
	if (array != NULL)
	{
		delete[] array[0][0];
		delete[] array[0];
		delete[] array;
		array = NULL;
	}

	lowx = lx;
	lowy = ly;
	lowz = lz;
	hix = hx;
	hiy = hy;
	hiz = hz;
	sizex = nx;
	sizey = ny;
	sizez = nz;

	// 1 cell added to cope with fractional intervals.
	howx = int ((hx - lx) / double(sizex) + 1);
	howy = int ((hy - ly) / double(sizey) + 1);
	howz = int ((hz - lz) / double(sizez) + 1);

	cx = (sizex - 1) / (hx - lx);
	cy = (sizey - 1) / (hy - ly);
	cz = (sizez - 1) / (hz - lz);

	_alloc ();

	max = 0;
	dropped = 0;
	dropped_std = 0;
}

template <class T> CStaticLut3D<T>::~CStaticLut3D(void)
{
	if (array != NULL)
	{
		delete[] array[0][0];
		delete[] array[0];
		delete[] array;
	}
}

template <class T> void CStaticLut3D<T>::operator=(T ini)
{
	const int size = sizex * sizey * sizez;
	T *tmp = array[0][0];
	for (int i = 0; i < size; i++)
	{
		*tmp++ = ini;
	}

	max = ini;
	dropped = 0;
	dropped_std = 0;
}

template <class T> void CStaticLut3D<T>::operator/=(T val)
{
	const int size = sizex * sizey * sizez;
	T *tmp = array[0][0];
	for (int i = 0; i < size; i++)
	{
		*tmp++ /= val;
	}

	max /= val;
}

template <class T> T& CStaticLut3D<T>::operator()(double x, double y, double z)
{
	// convert index.
	int xx = 0, yy = 0, zz = 0;

	xx = int ((x - lowx) * cx + .5);
	yy = int ((y - lowy) * cy + .5);
	zz = int ((z - lowz) * cz + .5);
	_ASSERT (xx >= 0 && xx < sizex && yy >= 0 && yy < sizey && zz >= 0 && zz < sizez);

	return array[xx][yy][zz];
}

template <class T> T& CStaticLut3D<T>::operator()(int i, int j, int k)
{
	_ASSERT (i >= 0 && i < sizex && j >= 0 && j < sizey && k >= 0 && k < sizez);
	return array[i][j][k];
}

template <class T> inline double CStaticLut3D<T>::GetX(int i) const
{
	_ASSERT (i >= 0 && i < sizex);
	return i / cx + lowx;
}

template <class T> inline double CStaticLut3D<T>::GetY(int j) const
{
	_ASSERT (j >= 0 && j < sizey);
	return j / cy + lowy;
}

template <class T> inline double CStaticLut3D<T>::GetZ(int k) const
{
	_ASSERT (k >= 0 && k < sizez);
	return k / cz + lowz;
}

template <class T> inline int CStaticLut3D<T>::GetIndexX(double x) const
{
	_ASSERT (x >= lowx && x <= hix);
	return int ((x - lowx) * cx + .5);
}

template <class T> inline int CStaticLut3D<T>::GetIndexY(double y) const
{
	_ASSERT (y >= lowy && y <= hiy);
	return int ((y - lowy) * cy + .5);
}

template <class T> inline int CStaticLut3D<T>::GetIndexZ(double z) const
{
	_ASSERT (z >= lowz && z <= hiz);
	return int ((z - lowz) * cz + .5);
}


//
//
//
//
//
//
//
//
//
// CStaticLut 1 D.
template <class T> class CStaticLut1D  
{
private:
	double lowx, hix;
	int sizex;
	double howx;
	double cx;
	T max;

	T *array;

	CStaticLut1D(const CStaticLut1D<T>&);
	double dropped;
	double dropped_std;

public:
	CStaticLut1D(void);
	CStaticLut1D(double lx, double hx, int nx);
	void Resize(double lx, double hx, int nx);
	virtual ~CStaticLut1D(void);

	void operator= (const CStaticLut1D<T>&);
	void operator=(T ini);
	void operator/=(T val);
	T& operator()(double x);
	T& operator()(int i);

	inline double GetX(int i) const;
	inline int GetIndexX(double x) const;
	inline int GetWidth(void) const { return sizex; }
	inline T* GetArrayPtr(void) { return array; }
	inline T& GetMaximum(void) { return max; }

	inline double& GetDropped(void) { return dropped; }
	inline double& GetDroppedStd(void) { return dropped_std; }
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T> void CStaticLut1D<T>::operator= (const CStaticLut1D<T>& src)
{
	if (this == &src)
		return;

	if (src.sizex != sizex)
	{
		if (array != NULL)
		{
			delete[] array;
		}

		sizex = src.sizex;

		// alloc.
		array = new T [sizex];
		_ASSERT (array != NULL);
	}
	else
	{
		sizex = src.sizex;
	}

	// data.
	memcpy (array[0], src.array[0], sizeof(T) * sizex);

	lowx = src.lowx;
	hix = src.hix;
	howx = src.howx;
	cx = src.cx;

	max = src.max;
	dropped = src.dropped;
	dropped_std = src.dropped_std;
}

template <class T> CStaticLut1D<T>::CStaticLut1D(void)
{
	lowx = hix = 0;
	sizex = 0;
	howx = 0;
	cx = 0;
	dropped = 0;
	dropped_std = 0;

	array = NULL;
	max = 0;
}

template <class T> CStaticLut1D<T>::CStaticLut1D (
						double lx, 
						double hx, 
						int nx)
{
	lowx = lx;
	hix = hx;
	sizex = nx;

	// 1 cell added to cope with fractional intervals.
	howx = int ((hx - lx) / double(sizex) + 1);

	cx = (sizex - 1) / (hx - lx);

	// alloc.
	array = new T [sizex];
	_ASSERT (array != NULL);

	max = 0;
	dropped = 0;
	dropped_std = 0;
}

template <class T> void CStaticLut1D<T>::Resize ( double lx, 
												double hx, 
												int nx)
{
	if (array != NULL)
	{
		delete[] array;
		array = NULL;
	}

	lowx = lx;
	hix = hx;
	sizex = nx;

	// 1 cell added to cope with fractional intervals.
	howx = int ((hx - lx) / double(sizex) + 1);

	cx = (sizex - 1) / (hx - lx);

	// alloc.
	array = new T [sizex];
	_ASSERT (array != NULL);

	max = 0;
	dropped = 0;
	dropped_std = 0;
}

template <class T> CStaticLut1D<T>::~CStaticLut1D(void)
{
	if (array != NULL)
	{
		delete[] array;
	}
}

template <class T> void CStaticLut1D<T>::operator=(T ini)
{
	const int size = sizex;
	T *tmp = array;
	for (int i = 0; i < size; i++)
	{
		*tmp++ = ini;
	}

	max = ini;
	dropped = 0;
	dropped_std = 0;
}

template <class T> void CStaticLut1D<T>::operator/=(T val)
{
	const int size = sizex;
	T *tmp = array;
	for (int i = 0; i < size; i++)
	{
		*tmp++ /= val;
	}

	max /= val;
}

template <class T> T& CStaticLut1D<T>::operator()(double x)
{
	// convert index.
	int xx = 0;

	xx = int ((x - lowx) * cx + .5);
	_ASSERT (xx >= 0 && xx < sizex);

	return array[xx];
}

template <class T> T& CStaticLut1D<T>::operator()(int i)
{
	_ASSERT (i >= 0 && i < sizex);
	return array[i];
}

template <class T> inline double CStaticLut1D<T>::GetX(int i) const
{
	_ASSERT (i >= 0 && i < sizex);
	return i / cx + lowx;
}

template <class T> inline int CStaticLut1D<T>::GetIndexX(double x) const
{
	_ASSERT (x >= lowx && x <= hix);
	return int ((x - lowx) * cx + .5);
}

//
//
//
//
//
//
//


#ifdef _DEFINED_HERE

#	undef _ASSERT
#	undef _DEFINED_HERE

#endif

#endif // !defined(AFX_STATICLUT_H__65AA4E11_D319_11D2_A4BD_0060087AA09D__INCLUDED_)
