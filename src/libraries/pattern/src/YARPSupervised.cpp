#include <stdio.h>
#include <list>

#include <math.h>

#include "YARPSupervised.h"
#include "YARPClimber.h"


class SuperPair : public YARPEvaluator
{
public:
  YARPSupervised& c0, c1;
  int index;
  SuperPair(YARPSupervised& ys0, YARPSupervised& ys1, int n_index) :
    c0(ys0), c1(ys1)
    {
      index = n_index;
    }

  virtual double Apply(double x)
    {
      double err = 0;
      double threshold = x;
      err += c0.Evaluate(index,threshold,1);
      err += c1.Evaluate(index,threshold,-1);
      double err2 = 0;
      err2 += c0.Evaluate(index,threshold,-1);
      err2 += c1.Evaluate(index,threshold,1);
      if (err2<err) err=err2;
      return err;
    }
};


void YARPSupervised::Set(std::istream& is)
{
  list<double> nums;
  char buf[256] = "0";
  int working = 0;
  int index = 0;
  int col_count = 0;
  int period = -1;
  int row_count = 0;
  while (!is.eof())
    {
      char ch = ' ';
      ch = is.get();
      if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t' || is.eof())
	{
	  if (working)
	    {
	      double v = 0;
	      v = atof(buf);
	      index = 0;
	      working = 0;
	      //fprintf(stderr,">>> Got %g\n", v);
	      nums.push_back(v);
	      col_count++;
	    }
	  if (ch == '\n' || ch == '\r' || is.eof())
	    {
	      if (col_count>0)
		{
		  if (period<0)
		    {
		      period = col_count;
		    }
		  if (col_count!=period)
		    {
		      fprintf(stderr,"Wanted %d, got %d, on row %d\n",
			      period, col_count, row_count);
		    }
		  assert(col_count==period);
		  col_count = 0;
		  row_count++;
		}
	    }
	}
      else
	{
	  working = 1;
	  buf[index] = ch;
	  buf[index+1] = '\0';
	  index++;
	  assert(index<sizeof(buf)-5);
	}
    }
  fprintf(stderr,">>> period is   %d\n", period);
  fprintf(stderr,">>> rowcount is %d\n", row_count);
  //data.Create(row_count,period);
  data.clear();
  data.resize(row_count);
  list<double>::iterator it = nums.begin();
  for (int i=0; i<row_count; i++)
    {
      data[i].resize(period);
      for (int j=0; j<period; j++)
	{
	  assert(it!=nums.end());
	  //data(i,j) = (*it);
	  //printf(">>> %d %d \n", i, j);
	  data[i][j] = (*it);
	  it++;
	}
    }
}

void YARPSupervised::Show(std::ostream& os)
{
  for (int i=0; i<data.size(); i++)
    {
      for (int j=0; j<data[i].size(); j++)
	{
	  os << data[i][j] << " ";
	}
      os << endl;
    }
}



void YARPSupervised::GetMeanStd(double_vector& mu, double_vector& sigma)
{
  assert(data.size()>0);
  int w = data[0].size();
  mu.resize(w);
  sigma.resize(w);
  for (int i=0; i<w; i++)
    {
      mu[i] = sigma[i] = 0;
    }
  for (int i=0; i<data.size(); i++)
    {
      double_vector& row = data[i];
      for (int j=0; j<w; j++)
	{
	  double v = row[j];
	  mu[j] += v;
	  sigma[j] += v*v;
	}
    }
  for (int i=0; i<w; i++)
    {
      mu[i] /= data.size();
      sigma[i] /= data.size();
      sigma[i] = sigma[i] - mu[i]*mu[i];
      sigma[i] = sqrt(sigma[i]);
      //cout << mu[i] << "/" << sigma[i] << " ";
    }
  //cout << endl;
}

void YARPSupervised::Filter(int index, double lo, double hi, 
			    YARPSupervised& target)
{
  int match = 0;
  target.Clear();
  for (int i=0; i<data.size(); i++)
    {
      double_vector& row = data[i];
      double v = row[index];
      if (v>=lo && v<=hi)
	{
	  match++;
	}
    }
  target.data.resize(match);
  match = 0;
  for (int i=0; i<data.size(); i++)
    {
      double_vector& row = data[i];
      double v = row[index];
      if (v>=lo && v<=hi)
	{
	  target.data[match] = row;
	  match++;
	}
    }  
}


void YARPSupervised::Distinguish(YARPSupervised& src, double_vector& w, 
				 double_vector& o, double_vector& rate)
{
  double tiny = 0.00001;
  double_vector mu1, mu2, sigma1, sigma2;
  GetMeanStd(mu1,sigma1);
  src.GetMeanStd(mu2,sigma2);
  int len = mu1.size();
  w.resize(len);
  o.resize(len);
  rate.resize(len);
  for (int i=0; i<len; i++)
    {
      w[i] = fabs(mu1[i]-mu2[i])/(fabs((sigma1[i]+sigma2[i])/2)+tiny);
      o[i] = 0;
      SuperPair sp(*this,src,i);
      YARPClimber climber;
      o[i] = climber.Apply(sp,mu1[i],mu2[i],10,10);
      double v = sp.Apply(o[i]);
      int sz = data.size()+src.data.size();
      if (sz<=0) sz = 1;
      v /= sz;
      /*
      if (w[i]>0.5/tiny)
	{
	  cout << "[" << i << "] ";
	}
      else
	{
	  cout << "[" << i << "|" << v << "] ";
	}
      */
      rate[i] = 1-v;
    }
  //cout << endl;
}


double YARPSupervised::Evaluate(int index, double threshold, double weight)
{
  int errors = 0;
  for (int i=0; i<data.size(); i++)
    {
      if (data[i][index]*weight>=threshold*weight)
	{
	  errors++;
	}
    }
  return errors;
}

