#ifndef YARPCLIMBER_INC
#define YARPCLIMBER_INC

class YARPEvaluator
{
public:
  virtual double Apply(double x) = 0;
};

class YARPClimber
{
public:
  double Apply(YARPEvaluator& eval, double x0, double x1, double steps = 10,
	       int recurse = 5);
};

#endif
