// VisDMatrix.inl
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved


#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif // _DEBUG

//
// classe CHmgTrsf.
//

//
// Questo e' altamente inefficiente.
//
inline C3dVector CHmgTrsf::Mpy(const C3dVector& v) const 
{
	C3dVector ret;
	ret=Rotation()*v+Position();
	return ret;
}

inline void CHmgTrsf::Zero(void) { (*this)=0.0; }

inline void CHmgTrsf::Identity(void)
{ 
	(*this)=0.0;
	(*this)(1,1)=1.0;
	(*this)(2,2)=1.0;
	(*this)(3,3)=1.0;
	(*this)(4,4)=1.0;
}

//
// Anche questa operazione e' una porcheria.
//	
inline C3dVector CHmgTrsf::operator*(const C3dVector& v) const 
{ 
	return Mpy(v); 
}

inline CHmgTrsf CHmgTrsf::operator*(const CHmgTrsf& h) 
{ 
	return CVisDMatrix::operator*(h); 
}
	
//
// classe CRotMatrix.
//
inline void CRotMatrix::Zero(void) 
{ 
	(*this)=0.0; 
}

inline void CRotMatrix::Identity(void) 
{
	(*this)=0.0;
	(*this)(1,1)=1.0;
	(*this)(2,2)=1.0;
	(*this)(3,3)=1.0;
}

//
// classe C3dVector.
//
inline double C3dVector::Scalar(const C3dVector& v) const
{
	double ret=0;
	ret+=(*this)(1)*v(1);
	ret+=(*this)(2)*v(2);
	ret+=(*this)(3)*v(3);
	return ret;
}

//
// classe CDiff.
//
inline CDiff& CDiff::TransCDiff(const CHmgTrsf& t)  // trasformazione di coordinate
{
	return *this;
	// da implementare!
#if 0
	Matrix jt(6,6);
	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	(*this)=jt*(*this);
	return *this;
#endif
}

//
// classe CForce.
//
inline CForce& CForce::TransForce(const CHmgTrsf& t)
{
	return *this;
	// da implementare!
#if 0
	Matrix jt(6,6);
	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	(*this)=jt.t()*(*this);
	return *this;
#endif
}

#ifdef _DEBUG
#undef new
#endif // _DEBUG