#include <yarp/YARPMath.h>
#include <cassert>
#include <functional>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>

YMatrix identity(int dim);
YMatrix ones(int dim);
YMatrix ones(int dim1, int dim2);
YMatrix diag(YVector val);
YVector conversion(YMatrix m );
YVector times(YVector v1, YVector v2);
YVector square(YVector v1);
YMatrix times(YMatrix m1, YMatrix m2);
YMatrix square(YMatrix m);
YVector ratio(YVector v1, YVector v2);
void sortGr(YVector & v);
YVector diag(YMatrix m);
YVector Row(YMatrix m, int num);
YVector Col(YMatrix m, int num);
YMatrix conversionR(YVector v);
YMatrix conversionC(YVector v);
double maximum (YVector v);
YVector max_col(YMatrix m);
YVector onesV(int dim);
double sumTot(YMatrix m);
YMatrix exp (YMatrix m);
YMatrix abs (YMatrix m);
YMatrix log (YMatrix m);
double maximum(YMatrix m);
double minimum (YVector v);
double minimum (YMatrix m);
YVector unroll(YMatrix m);
YMatrix roll(YVector v, int numR, int numC);
YMatrix PartPos(YMatrix m);
void choldc(YMatrix & a, int n);
void ReplSup(YMatrix & m, double val);
YMatrix segno(YMatrix m);
YMatrix PartNeg(YMatrix m);
YMatrix ratio(YMatrix m1, YMatrix m2);
double prod(YMatrix m);
double prod(YVector v);
int findMag(YVector v, double val, int inds[]);
void outIni(YVector & v, std::ostream & file);
void outIni(YMatrix & m, std::ostream & file);
double sumTot(const YVector& v);
double mean(const YVector& v);
double var(const YVector& v);
double num(const YVector& v);



