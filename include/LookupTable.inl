/////////////////////////////////////////////////////////////////////////////
//
// __CGTable.inl
//	__CGTable inlines.
//
//

inline void __CTableUnit::operator=(const __CTableUnit& u)
{
	nelem = u.nelem;
	x = u.x;
	y = u.y;
}

inline CVisDVector& __CTableUnit::gety(void)
{
	return y;
}

inline CVisDVector& __CTableUnit::getx(void)
{
	return x;
}

inline void __CTableUnit::reset(void)
{
	nelem = 0;
}

// gradient descent su X.
inline void __CTableUnit::add(const CVisDVector& nx, const CVisDVector& ny,double eta)
{
	if (nelem == 0)
	{
		y = ny;
		x = nx;
		nelem++;
	}
	else
	{
		assert (ny.Length() == y.Length());
		assert (nx.Length() == x.Length());
		y = y * nelem + ny;
		nelem++;
		y /= nelem;

		// mi avvicino all'ultimo punto richiesto.
		x += eta * (nx - x);
	}
}

inline void __CTableUnit::add_delta(const CVisDVector& nx, const CVisDVector& delta)
{
	if (nelem == 0)
	{
		y = delta;
		x = nx;
		nelem++;
	}
	else
	{
		assert (delta.Length() == y.Length());
		assert (nx.Length() == x.Length());
		y += delta;
		nelem++;

		// mi avvicino all'ultimo punto richiesto.
		//x += eta * (nx - x);
	}
}

/////////////////////////////////////////////////////////////////////////////
//

inline double __CGTable::LastError(void) const { return lasterror; }

inline void __CGTable::ChangeResolution (double newres) { threshold = newres * newres; }
inline double __CGTable::GetResolution (void) { return sqrt(threshold); }

inline int __CGTable::GetNumberOfSamples (void) const { return n_steps; }
