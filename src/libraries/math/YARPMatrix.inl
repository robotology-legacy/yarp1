// VisDMatrix.inl
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved


#ifndef __QNX__
#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif // _DEBUG

#else
	// QNX specific stuff here.
#endif


inline bool CVisDVector::operator!=(const CVisDVector& refvector) const
{
    return !(*this == refvector);
}

inline bool CVisDMatrix::operator!=(const CVisDMatrix& refmatrix) const
{
    return !(*this == refmatrix);
}

inline bool CVisDVector::operator!=(double dbl) const
{
    return !(*this == dbl);
}

inline bool CVisDMatrix::operator!=(double dbl) const
{
    return !(*this == dbl);
}

inline CVisDMatrix& CVisDMatrix::Invert(void)
{
	*this = Inverted();

	return *this;
}

inline CVisDMatrix& CVisDMatrix::Transpose(void)
{
	*this = Transposed();

	return *this;
}


//
// Self-describing stream output.
//
// LATER:  We should consider having both op<< and op>> with these streams.
//
#ifdef VIS_INCLUDE_SDSTREAM
inline CVisSDStream& operator<<(CVisSDStream& s, CVisDVector& o)
	{ return VisSDReadWriteClass(s, o); }

inline CVisSDObject<CVisDVector>& CVisSDObject<CVisDVector>::Exemplar(void)
{
	// Global variable used with self-describing streams.
	// LATER:  Find a way to avoid using this global variable.
	extern VisMatrixExport CVisSDObject<CVisDVector> g_visdvectorExemplar;

	return g_visdvectorExemplar;
}


inline CVisSDStream& operator<<(CVisSDStream& s, CVisDMatrix& o)
	{ return VisSDReadWriteClass(s, o); }

inline CVisSDObject<CVisDMatrix>& CVisSDObject<CVisDMatrix>::Exemplar(void)
{
	// Global variable used with self-describing streams.
	// LATER:  Find a way to avoid using this global variable.
	extern VisMatrixExport CVisSDObject<CVisDMatrix> g_visdmatrixExemplar;

	return g_visdmatrixExemplar;
}
#endif

#ifndef __QNX__
#ifdef _DEBUG
#undef new
#endif // _DEBUG
#endif
