#ifndef _REAL
#define _REAL
typedef double REAL;
#endif

#ifndef _BOOL
#define _BOOL
typedef int   BOOL;
#endif

#ifndef _H_MBP
#define _H_MBP

#define OR      ||
#define AND     &&
#define NOT     !

#define EQ      ==
#define GE      >=
#define GT      >
#define LE      <=
#define LT      <
#define NE      !=

#define MBP_ERROR   -1
#define MBP_NO_ERROR 0

#define TEXT_LINE 81

#ifndef TRUE
#define TRUE    -1
#endif

#ifndef FALSE
#define FALSE    0
#endif

#ifndef MAXFLOAT
#define MAXFLOAT 1.0e38F
#endif

#define ERROR_MSG(x)    {\
			 printf("ERROR: %s\n",x);\
			 exit(-1);\
			}

#define DEBUG(x) printf("***DEBUG*** %s\n",x);\
		 getch();

#define WRITE_MAT(Mat,Rows,Cols,Name) {\
					printf("*** MATRIX %s ***\n",Name);\
					for (ii=0; ii LT (Rows); ii++)\
					 {\
					  for (jj=0; jj LT (Cols); jj++)\
					   printf(" %4.2f ",Mat[ii*(Cols)+jj]);\
					  printf("\n");\
					 }\
				      }

#define CHECK_PTR(x) if (x EQ NULL)\
		      {\
		       printf("Insufficient memory\n");\
		      }

void RandomWeights (REAL*, int, int, REAL);

void FeedForward(REAL *, int, int, REAL *, int, int,
		          REAL *, int, int, REAL *, int);

REAL f(REAL);
REAL df(REAL);

void EBPLastLayer(REAL *,int, int, REAL *,int, int, REAL *, int, int);
void EBP(REAL *, int, int, REAL *, int, int, REAL *, int, int, REAL *, int, int);

void Step(REAL *, int , int , REAL *, int , int , REAL *, int , int , REAL *, int);

REAL ComputeDigitalCost(REAL *, int, int, REAL *, int, int);
REAL ComputeAnalogCost(REAL *, int, int, REAL *, int, int);
REAL ComputeMaximumCost(REAL *, int, int, REAL *, int, int);

REAL ComputeGradientNorm(REAL **, REAL **, int *, int);

void PrintStep(char *, long, REAL, REAL, REAL, REAL);

void OutTime(long, int, int, int *, double);

void BackStep(REAL *, int, int, REAL *, int, REAL *, int, int, REAL *, int, 
			  REAL *, int, int, REAL *, int, REAL *, int, int, REAL *, int);

void UpdateWeights (REAL *, int, int, REAL *, int, REAL, REAL, REAL *, int, int,
					REAL *, int, REAL *, int, int, REAL *, int, REAL *, int,
					int, REAL *, int);


#endif
