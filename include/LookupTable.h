	
// LookupTable.h : Declaration of the CLookupTable

#ifndef __LOOKUPTABLE_H_
#define __LOOKUPTABLE_H_

#include <stdio.h>

#include <VisMatrix.h>

#	ifdef WIN32

#include <vector>
using namespace std;

#include <tchar.h>
#define fopen _tfopen
#define fprintf _ftprintf
#define fscanf _ftscanf

#	endif

#	ifdef __QNX__

#include <YARPSafeNew.h>
#include <vector.h>				// stl list

#ifndef _TCHAR 
#define _TCHAR char
#endif

#ifndef _T
#define _T(x) x
#endif

#else

#include <vector>				// stl list
using namespace std;

#	endif	// __QNX__

#if 0
#ifndef _TCHAR 
#define _TCHAR char
#endif

#ifndef _T
#define _T(x) x
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
//
// class __CTableUnit.
//	look-up table unit. x and y are in and out.
//
class __CTableUnit {
private:
	int nelem;

	CVisDVector x;
	CVisDVector y;

public:
	__CTableUnit(void);
	__CTableUnit(const __CTableUnit&);
	virtual ~__CTableUnit(void) {}

	inline void operator=(const __CTableUnit&);
	inline void reset(void);

	inline CVisDVector& gety(void);
	inline CVisDVector& getx(void);
	inline int& getnelem(void) { return nelem; }

	inline void add(const CVisDVector&,
					const CVisDVector&,
					double);
	inline void add_delta(const CVisDVector&,
						  const CVisDVector&);
};

//
// class __CGTable.
//
class __CGTable : public vector<__CTableUnit>
{
private:
	__CGTable(const __CGTable&);
	void operator=(const __CGTable&);

	int dimx;
	int	dimy;

	CVisDVector zero; // solo per il return nel caso di lista vuota.
	double lasterror; // errore minimo dopo una call a Nearest.
	typedef vector<__CTableUnit>::iterator POSITION;
	POSITION imin;    // posizione del minimo.
	double threshold; // soglia sull'errore.
	double eta;		  // step per tuning posizione celle.

	// temporanei.
	__CTableUnit  s_temp;
	CVisDVector   v_temp;
	CVisDVector   vv_temp;

	int n_steps;	  // number of training samples processed.

public:
	__CGTable(void);
	__CGTable(int, int, double, double);
	~__CGTable(void);

	void Rebuild(int, int, double, double);

	CVisDVector Nearest(const CVisDVector& x);
	void CheckAndAdd(const CVisDVector& x, const CVisDVector& y);
	void AddDelta(const CVisDVector& x, const CVisDVector& delta);
	void AddNew(const CVisDVector& x, const CVisDVector& y);
	int GetRandom(CVisDVector& x, CVisDVector& y);
	
	bool DumpToFile(const _TCHAR *filename) const;
	bool LoadFromFile(const _TCHAR *filename);
	
	inline double LastError(void) const;
	inline void ChangeResolution(double);
	inline double GetResolution(void);
	inline int GetNumberOfSamples(void) const;
};

/////////////////////////////////////////////////////////////////////////////
// CLookupTable
//
// this crazy stuff is a residual from a DCOM porting.
#define XX_STDMETHOD(x) x
#define XX_STDMETHODIMP int

#ifndef XX_S_OK
#define XX_S_OK 0
#endif

#ifndef XX_E_UNEXPECTED
#define XX_E_UNEXPECTED -1
#endif

class CLookupTable
{
public:
	__CGTable m_map;
	CVisDVector m_tmp_x;
	CVisDVector m_tmp_y;

public:
	CLookupTable()
	{
	}

// ILookupTable
public:
	XX_STDMETHOD(Rebuild)(/*[in]*/ int dx, /*[in]*/ int dy, /*[in]*/ double thr);
	XX_STDMETHOD(GetResolution)(/*[out]*/ double *res);
	XX_STDMETHOD(GetNumberOfSamples)(/*[out,retval]*/ int *samples);
	XX_STDMETHOD(ChangeResolution)(/*[in]*/ double newres);
	XX_STDMETHOD(GetLastError)(/*[out,retval]*/ double *error);
	XX_STDMETHOD(AddDelta)(/*[in]*/ int size, /*[in,size_is(size)]*/ double * v_in,/*[in]*/ int size2,/*[in,size_is(size2)]*/ double * v_out);
	XX_STDMETHOD(AddNew)(/*[in]*/ int size,/*[in,size_is(size)]*/ double *v_in, /*[in]*/ int size2,/*[in,size_is(size2)]*/ double *v_out);
	XX_STDMETHOD(GetRandom)(/*[in]*/ int size,/*[out,size_is(size)]*/ double *v_in, /*[in]*/ int size2, /*[out,size_is(size2)]*/ double *v_out);
	XX_STDMETHOD(DumpToFile)(/*[in]*/ int size,/*[in,size_is(size)]*/ _TCHAR *filename);
	XX_STDMETHOD(LoadFromFile)(/*[in]*/ int size,/*[in,size_is(size)]*/ _TCHAR *filename);
	XX_STDMETHOD(CheckAndAdd)(/*[in]*/ int size, /*[in,size_is(size)]*/ double *v_in, /*[in]*/ int size2, /*[in,size_is(size2)]*/ double *v_out);
	XX_STDMETHOD(Nearest)(/*[in]*/ int size, /*[in,size_is(size)]*/ double *v_in, /*[in]*/ int size2, /*[out,size_is(size2)]*/ double *v_out );
	XX_STDMETHOD(Init)(/*[in]*/ int n,/*[in]*/ int m,/*[in]*/ double res);
};

#include "LookupTable.inl"

#endif //__LOOKUPTABLE_H_
