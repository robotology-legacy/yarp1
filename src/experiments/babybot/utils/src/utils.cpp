#include "yarp/utils.h"

//--------------------------------------------------
// Crea matrice identità quadrata di dimensione dim.
//--------------------------------------------------

using namespace std;

YMatrix identity(int dim)
{

	YMatrix ident(dim,dim);

	for(int i=1;i<=dim;i++)	ident(i,i) = 1.0;

	return ident;	
}

//--------------------------------------------------
// Crea vettore di 1 di dimensione dim.
//--------------------------------------------------

YVector onesV(int dim)
{
	YVector v(dim);
	v = 1.0;
	return v;
}

//--------------------------------------------------
// Crea matrice di 1 quadrata di dimensione dim.
//--------------------------------------------------

YMatrix ones(int dim)
{

	YMatrix o(dim,dim);

	o = 1.0 ;

	return o;	

}

//--------------------------------------------------
// Crea matrice di 1 di dimensioni dim1, dim2.
//--------------------------------------------------

YMatrix ones(int dim1, int dim2)
{

	YMatrix o(dim1,dim2);

	o = 1.0 ;

	return o;	

}

//--------------------------------------------------
// Dato un vettore, crea una matrice che ha i valori
// di quel vettore sulla diagonale.
//--------------------------------------------------

YMatrix diag(YVector val)
{
	int dim = val.Length();

	YMatrix m(dim,dim);

	for(int i=1;i<=dim;i++)		m(i,i) = val(i);
	
	return m;
}

//-------------------------------------------------------
// Data una matrice, ne estrae la diagonale in un vettore
//-------------------------------------------------------

YVector diag(YMatrix m)
{
	assert(m.NCols() == m.NRows());

	int n = m.NCols();

	YVector y(n);

	for (int i=0; i<n; i++)
		
		y.data()[i]= m.data()[i][i];
	
	return y;
}

//------------------------------------------------------------------------
// Data una matrice con una dimensione unitaria, la converte in un vettore
//------------------------------------------------------------------------

YVector conversion(YMatrix m )
{
	YVector v;
	if ( m.NCols() == 1)
			v.Resize(m.NRows(),*m.data());
	else if (m.NRows() == 1)
			v.Resize(m.NCols(),*m.data());

	return v;


}

//-------------------------------------------------------
// Dato un vettore lungo n, lo trasforma in una matrice di 
// dimensioni (1,n) 
//-------------------------------------------------------

YMatrix conversionR(YVector v)
{ 
	YMatrix m(1,v.Length(),v.data());
	return m;
}

//-------------------------------------------------------
// Dato un vettore lungo n, lo trasforma in una matrice di 
// dimensioni (n,1) 
//-------------------------------------------------------

YMatrix conversionC(YVector v)
{ 
	YMatrix m(v.Length(),1,v.data());
	return m;
}

//-------------------------------------------------------
// Ritorna la matrice ottenuta calcolando il prod. elemento
// per elemento di due altre matrici di uguali dim.
//-------------------------------------------------------


YMatrix times(YMatrix m1, YMatrix m2)
{

	assert(m1.NCols() == m2.NCols() && m1.NRows() == m2.NRows());

	YMatrix mret(m1.NRows(),m1.NCols());
	int i,j;

		for(i=0; i < m1.NRows(); i++)
		
			for(j=0; j < m1.NCols(); j++)
				
				mret[i][j] = m1[i][j]*m2[i][j];

		
	return mret;

}

//---------------------------------------------------------
// Ritorna il vettore ottenuto calcolando il prod. elemento
// per elemento di due altri vettori di uguale lunghezza.
//---------------------------------------------------------

YVector times(YVector v1, YVector v2)
{

	assert(v1.Length()==v2.Length());

	YMatrix m = diag(v2);

	YVector v = m*v1;

	return v;

}

//---------------------------------------------------------
// Ritornano il vettore o la matrice, ottenuti calcolando 
// il rapp. elemento per elemento di due altri vettori o di
// due altre matrici di uguali dimensioni.
//---------------------------------------------------------

YVector ratio(YVector v1, YVector v2)
{

	assert(v1.Length()==v2.Length());

	for(int i=1 ; i <= v1.Length(); i++) 
	{ 
	  assert(v2(i) != 0);
	  v2(i) = 1/ v2(i);
	}
   
	YVector v(v1.Length());
	v = times(v1,v2);
	
	return v;
}


YMatrix ratio(YMatrix m1, YMatrix m2)
{

	assert(m1.NCols()==m2.NCols()&& m1.NRows() == m2.NRows());

	YVector v1 = unroll(m1);
	YVector v2 = unroll(m2);

	YVector v = ratio(v1,v2);
	
	return roll(v,m1.NRows(),m1.NCols());
}

//------------------------------------------------------------
// Ritornano il vettore o la matrice ottenuti calcolando il 
// quadrato elemento per elemento di un vettore o di una mat.
//------------------------------------------------------------

YVector square(YVector v1)
{
	v1 = times(v1,v1);
	return v1;
}


YMatrix square(YMatrix m)
{
	m = times(m,m);
	return m;
}

//------------------------------------------------------------
// Ordina un vettore in modo decrescente
//------------------------------------------------------------

void sortGr(YVector & v)
{

sort(v.data(),v.data()+ v.Length(), greater <double> ());

}

//------------------------------------------------------------
// Estrae la riga num dalla matrice m in un vettore
//------------------------------------------------------------

YVector Row(YMatrix m, int num)
{
	// si poteva realizzare anche trasponendo la matrice da cui estraevo la colonna

	assert(num <= m.NRows());
	YVector row(m.NCols(),*(m.data()+(num-1)));
	return row;
}

//------------------------------------------------------------
// Estrae la colonna num dalla matrice m in un vettore
//------------------------------------------------------------

YVector Col(YMatrix m, int num)
{
	// postmoltiplico per un vett della base canonica
	
	assert(num <= m.NCols());

	YVector bc(m.NCols());
	bc(num)=1;	
	YVector col = m*bc;
	return col;
}

//------------------------------------------------------------
// Calcolano il massimo elemento di un vettore o di una matrice.
//------------------------------------------------------------

double maximum (YVector v)
{
  return *( max_element(v.data(), v.data() + v.Length()) );
   
}

double maximum(YMatrix m)
{
	YVector v = unroll(m);
	
	return maximum(v);
}


//------------------------------------------------------------------------
// Versione con divisione in colonne (utile per avere max su ogni colonna)
//------------------------------------------------------------------------

YVector max_col(YMatrix m)

{
	YVector v(m.NCols());
	
	for(int i=1; i<=m.NCols(); i++)
		v(i) = maximum(Col(m,i));

	return v;
}

//------------------------------------------------------------
// Calcolano il minimo elemento di un vettore o di una matrice.
//------------------------------------------------------------

double minimum (YVector v)
{
    return *( min_element(v.data(), v.data() + v.Length()) ) ;
   
}

double minimum(YMatrix m)
{
	YVector v = unroll(m);
	
	return minimum(v);
}

//---------------------------------------------------------------
// Calcola la somma degli elementi di un vettore o di una matrice.
//---------------------------------------------------------------

double sumTot(const YVector& v)
{
	return accumulate(v.data(),v.data() + v.Length(), 0.0f);
}


double sumTot(YMatrix m)
{
	return accumulate(*m.data(), *m.data() + m.NCols()*m.NRows(),0.0f);
}

//---------------------------------------------------------------
// Calcola l'esponenziale, elemento per elemento, di una matrice.
//---------------------------------------------------------------

YMatrix exp (YMatrix m)
{
	for(int i=0; i<m.NRows(); i++)
		for(int j=0; j<m.NCols(); j++)
			m.data()[i][j] = exp(m.data()[i][j]);

		return m;
}

//-------------------------------------------------------------------
// Calcola il valore assoluto, elemento per elemento, di una matrice.
//-------------------------------------------------------------------

YMatrix abs (YMatrix m)
{

for(int i=0; i<m.NRows(); i++)
		for(int j=0; j<m.NCols(); j++)
			m.data()[i][j] = fabs(m.data()[i][j]);

		return m;
}

//---------------------------------------------------------------
// Calcola il logaritmo, elemento per elemento, di una matrice.
//---------------------------------------------------------------

// Non fa controlli di positività!

YMatrix log (YMatrix m)
{

for(int i=0; i<m.NRows(); i++)
		for(int j=0; j<m.NCols(); j++)
			m.data()[i][j] = log(m.data()[i][j]);

		return m;
}

//--------------------------------------------------------------------------
// Funzioni di trasformazione tra vettore e matrice (in via di eliminazione)
//--------------------------------------------------------------------------

YVector unroll(YMatrix m)
{
	YVector v(m.NCols() * m.NRows(), *m.data());
	return v;
}

YMatrix roll(YVector v, int numR, int numC)
{
	assert (numR * numC <= v.Length());

	YMatrix m(numR,numC,v.data());
	
	return m;

}

//---------------------------------------------------------------
// Calcolano la parte positiva e negativa di una matrice.
//---------------------------------------------------------------

YMatrix PartPos(YMatrix m)
{
	return (m+abs(m))/2;
}

YMatrix PartNeg(YMatrix m)
{
	return (m-abs(m))/2;
}

//------------------------------------------------------------------
// Calcolano il prodotto degli elementi del vettore o della matrice.
//------------------------------------------------------------------

// Ritorna 0 se il vettore contiene zeri, e cmq ritorna sempre il prod di tutti gi elem.

double prod(YVector v)
{
	return accumulate(v.data(),v.data()+ v.Length(), 1.0, multiplies <double> ());
}


double prod(YMatrix m)
{
	return accumulate(*m.data(), *m.data() + m.NCols()*m.NRows(),1.0, multiplies <double> ());
}

//------------------------------------------------------------------
// Ritorna una matrice fatta di 1 (in corrispondenza di num pos in m)
// e di -1 (in corrispondenza di num neg in m)
//------------------------------------------------------------------

// Non ancora gestite matrici con 0 all'interno

YMatrix segno(YMatrix m)
{

		return ratio(m,abs(m));
}

//-------------------------------------------------------------------
// Sostituisce agli elementi di una matrice superiori in modulo ad un 
// certo valore tale valore, conservandone il segno originale.
//-------------------------------------------------------------------

void ReplSup(YMatrix & m, double val)
{
	for (int i=0; i<m.NCols()*m.NRows(); i++)
		 if (*(*m.data() + i)  > val)
			 *(*m.data() + i) = val;
		 else if ( -*(*m.data() + i) < -val)
			*(*m.data() + i) = -val;
 

}

//-----------------------------------------------------------------
// Cholesky factorization: trasforma a - def. pos. - in una matrice 
// triangolare sup tale che aT * a = a originale.
//-----------------------------------------------------------------

void choldc(YMatrix & a, int n)

/*
	Commento della versione originale tratta da Numerical Recipes

	Given a positive-de.nite symmetric matrix a[1..n][1..n], this routine constructs its Cholesky
	decomposition, A = L · LT . On input, only the upper triangle of a need be given; it is not
	modi.ed. The Cholesky factor L is returned in the lower triangle of a, except for its diagonal
	elements which are returned in p[1..n].
*/
  
{

int i,j,k;
double sum;
double * p;
p = new double[n+1];
for (i=1;i<=n;i++) 
	{
		for (j=i;j<=n;j++) 
		{
			for (sum=a(i,j),k=i-1;k>=1;k--) sum -= a(i,k)*a(j,k);
			if (i == j) 
			{
				if (sum <= 0.0) //a, with rounding errors, is not positive de.nite.
				{
					cout << "errore!" << endl;
					a = 0;
					return;
				}
				p[i]=sqrt(sum);
				a(i,i) = p[i]; 
			} 
			else
			{
				a(j,i)=sum/p[i];
				a(i,j) = 0;
			}
			
		}
	}

	a.Transpose();
	delete [] p;

}

//--------------------------------------------------------------------------------
// Ritorna il numero di occorrenze (val. vett > val) trovate nel vettore v e salva
// in inds[] gli indici corrisp. Anche in inds ragiona a partire da 1.
// E' consigliabile allocare un vettore di dim pari a v.Length()
// Non è per nulla ottimizzato!
//--------------------------------------------------------------------------------

int findMag(YVector v, double val, int inds[])

{
	int i,j=1;

	for (i=1; i<= v.Length(); i++)
		if ( v(i) > val)
			inds[j++] = i;
	
	return	j-1;

}

//-----------------------------------------------------------------
// Funzioni utili per l'output su file ini di vettori e matrici.
//-----------------------------------------------------------------

void outIni(YVector & v, ostream & file)
{
	for (int i=0; i<v.Length(); i++)
		file << v[i] << ' ' ;
	file << endl;
}

void outIni(YMatrix & m, ostream & file)
{
	for (int i=0; i<m.NCols()*m.NRows(); i++)
		file << *(*m.data() + i) << ' ' ;
	file << endl;
}


// ---------------------------------------------------------------------------------------- 
//Funzione che acquisisce i dati
// ---------------------------------------------------------------------------------------- 

void GetInput(istream & device, YVector& X, YVector& Y)
{
	int i;
	for(i = 1; i<= X.Length(); i ++)
			device >> X(i);
	
	for (i=1; i<= Y.Length(); i ++)
			device >> Y(i);

}


//---------------------------------------------------
// Calcola la media campione : somma N elementi / N.
//---------------------------------------------------


double mean(const YVector& v)
{

return (accumulate(v.data(),v.data() + v.Length(), 0.0f)/ v.Length());

}

//-----------------------------------------------------------
// Calcola la varianza campionaria:  somma N (x-xm)^2 / N-1.
//-----------------------------------------------------------

double var(const YVector& v)
{

	return sumTot(square(v - mean(v)))/(v.Length()-1);
}

//---------------------------------------------------------------
// Ritorna il primo elemento di un vettore. Utile per trasformare
// un vettore di dim 1 in un numero.
//---------------------------------------------------------------


double num(const YVector& v)
{
	return *v.data();
}



/*

// Versioni alternative di alcune funzioni.


void ReplSup(YMatrix & m, double val)
{

 m = times((PartNeg(abs(m) - val) + val),segno(m)) ;

}


// Versione con divisione in colonne (utile per avere min su ogni colonna)

double minimum(YMatrix m)
{
	YVector v(m.NCols());
	
	for(int i=1; i<=m.NCols(); i++)
		v(i) = minimum(Col(m,i));

	return minimum(v);
}

// Versione con for nestati

double sumTot(YMatrix m)
{
double sum = 0;
for(int i=0; i<m.NRows(); i++)
	for(int j=0; j<m.NCols(); j++)
		 sum = sum + *(*(m.data()+i)+j);
return sum;
}

// Versione con struttura a matrice

double sumTot(YMatrix m)
{
	YVector a,b;
	a = onesV(m.NRows());
	b = onesV(m.NCols());
	
	return a *( m * b);

}
*/
