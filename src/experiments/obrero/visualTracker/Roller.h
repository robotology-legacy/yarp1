
#include <math.h>

#include <deque>
using namespace std;


template <class T>
class Roller : public deque<T> {
public:
  int len;
  double total;
  double total2;

  Roller(int nlen=100) { len = nlen; clear(); }

  void setLen(int nlen) { len = nlen; clear(); }

  double sum() { return total; }

  void clear() { deque<T>::clear();  total2 = total = 0; }

  double mean() { if (size()==0) return 0; else return total/size(); }

  double mean2() { if (size()==0) return 0; else return total2/size(); }

  double var() { double m = mean();  return fabs(mean2()-m*m); }

  double std() { return sqrt(var()); }

  T& oldest() {
    return back();
  }

  void add(T t) {
    push_front(t);
    total += t;
    total2 += t*t;
    if (size()>len)
      {
	double b = back();
	total -= b;
	total2 -= b*b;
	pop_back();
      }
  }
};

