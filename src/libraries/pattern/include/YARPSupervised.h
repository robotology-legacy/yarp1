#ifndef YARPSUPERVISED_INC
#define YARPSUPERVISED_INC

#include <iostream>
#include "YARPSequence.h"

#include <vector>

typedef vector<vector<double> > double_matrix;
typedef vector<double> double_vector;

class YARPSupervised
{
protected:
  double_matrix data;
public:
  void Clear() { data.clear(); }

  void Set(std::istream& is);
  void Show(std::ostream& os);

  void Filter(int index, double lo, double hi, YARPSupervised& target);

  void Filter(int index, double val, YARPSupervised& target)
    { Filter(index, val-0.001, val+0.001, target);  }

  void Filter(double val, YARPSupervised& target)
    { Filter(0, val, target);  }

  void GetMeanStd(double_vector& mu, double_vector& sigma);

  void Distinguish(YARPSupervised& src, double_vector& w, double_vector& o,
		   double_vector& rate);

  double Evaluate(int index, double threshold, double weight);

};

#endif
