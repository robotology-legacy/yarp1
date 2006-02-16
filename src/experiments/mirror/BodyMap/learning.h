// learning.h : header file for a generic learning machine
//

#include "libsvm.h"

// how many samples do we ever consider at a time?
const int numOfSamples = 150;

// a learning machine

typedef class BodyMapLearningBlockType {

public:

	// constructor and destructor
	BodyMapLearningBlockType();
	~BodyMapLearningBlockType();

	// the samples, their values, and the means and stdvs for each dimension
	struct svm_node** sample;
	double *value;
	double mean[7], stdv[7];
	
	// current number of samples stored
	unsigned int sampleCount;

	// methods
	void predict(double, double, double, double*, double*, double*, double*);
	void train();
	bool addSample(double, double, double, double, double, double, double);

private:

	// svm-related structures
	struct svm_parameter param;
	struct svm_problem prob[4];
	struct svm_model* model[4];
	// sample to be predicted
	struct svm_node newSample[4];

} BodyMapLearningBlock;
