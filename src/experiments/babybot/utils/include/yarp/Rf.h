#ifndef RF1_H
#define RF1_H


#include <yarp/YARPMath.h>
#include <string>
#include "utils.h"
#include <fstream>
#include <yarp/YARPConfigFile.h>

class Rf {

	//friend ostream & operator<<( ostream &output, const Rf & r );	
	//	friend void cane(const Rf & r);

public:

	Rf(YVector c,YMatrix D);
	Rf(void);
	Rf(const Rf & templ);
	~Rf(){};

	void init(void);
	void init(Rf * templ, YVector myc, YVector myy, int n_in, int n_out, double init_P, double init_lambda);
    void init(YVector myc, YVector myy,YMatrix init_D, YMatrix init_M, YMatrix init_alpha, int n_in, int n_out, double init_P, double init_lambda);
	void print() const;
	void print(ofstream & file) const;
	
	void cancel(int num);

	double compute_weight(bool diag_only, std::string kernel, YVector x);
	double getW (void) const {return w;};
	YMatrix getD (void) const {return D;};
	YVector getmean_x(void) const {return mean_x;};
	YMatrix getB(void) const {return B;};
	YVector getb0(void) const {return b0;};

	void setW (double myw)  { w = myw;};
	void setE (double numE) { e = numE;};
	void setE (double * vecE, int d) { e.Resize(d,vecE);};

	bool isTrustworthy(void) const {return trustworthy;};
 
	// ritorna 0 se non è cancellato l'rf
	// ritorna altrimenti un numero che indica l'ordine in cui è stato effettuato il pruning
	int isErased(void) const {return erased;};
 
	 YVector getC (void) const {return c;}

	//void update_means(YVector xn,YVector yn, double myw, YVector & xmz, YVector & ymz);//*
	void update_means(YVector xn,YVector yn, double myw);

	//YVector update_regression(YVector x, YVector y, double myw,YVector & e_cv,YVector & e);//*
	YVector update_regression( double myw);
	void dist_derivatives(double myw,YVector dx,bool diag_only,std::string kernel,double penalty, bool meta,YMatrix & dwdM,YMatrix & dJ2dM,YMatrix & dwwdMdM,YMatrix & dJ2J2dMdM);
	//double update_distance_metric(YVector x, YVector y,double myw,YVector e_cv, YVector e,YVector xn, double pen, bool meta, double meta_rate, string kernel, bool diag_only);//**
	double update_distance_metric(double myw,YVector xn, double pen, bool meta, double meta_rate, std::string kernel, bool diag_only);
	void update_statistical(double myw, double mytau, double myfin);
	//void update_composite(double comp_alpha, double tmsj, double comp_sum_w, YVector xn, YVector e);//**
	void update_composite(double comp_alpha, double tmsj, double comp_sum_w, YVector xn);
	void SaveRf(std::ostream & file) ;
	int LoadRf(YARPConfigFile & file,char * title, int n_in, int n_out);

private:
	YMatrix D;
	YMatrix M;
	YMatrix alpha;
	YVector b0;
	YMatrix P;
	YMatrix B;
	YMatrix H;
	YMatrix R;
	YMatrix h;
	YMatrix b;
	YVector c;
	YVector vif;
	
	double sum_w;
	double sum_e_cv2;
	double sum_e2;
	double n_data;
	bool trustworthy;
	double lambda;
	int erased;

	YVector mean_x;
	YVector var_x;

	double w;

	// nuove variabili necessarie per il funzionamento della rete,
	// che vengono reinizializzate ad ogni Train

	YVector xmz;
	YVector ymz;
	YVector e;
	YVector e_cv;
};

#endif