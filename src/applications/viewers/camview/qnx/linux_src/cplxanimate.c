#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/* z=(x^2+5c^2)*x/|x+c|^3 */
void f(float x[2], float c[2], float z[2])
{
  float foo[2];
  float d;
  z[0]=x[0]+c[0];
  z[1]=x[1]+c[1];
  d=z[0]*z[0]+z[1]*z[1];
  if(d==0)
    {
      z[0]=100; z[1]=-99; return;
    }
  d=sqrt(d*d*d);
  z[0]=x[0]*x[0]-x[1]*x[1]+c[0]*c[0]-c[1]*c[1];
  z[1]=2*x[0]*x[1]+10*c[0]*c[1];
  foo[0]=z[0]*x[0]-z[1]*x[1];
  foo[1]=z[0]*x[1]+z[1]*x[0];
  z[0]=foo[0]/d;
  z[1]=foo[1]/d;
}


int main(int argc, char *argv[])
{
  int i,j,width,height;
  float x[2],c[2],z[2],d,theta;

  width=height=100;
  if(argc>=2)
    {
      width=atoi(argv[1]);
      if(argc>=3)
	height=atoi(argv[2]);
    }
  theta=0;
  while(1)
    {
      c[0]=sin(theta)*.66+cos(3*theta+.2)*sin(2.66*theta+1)*.34;
      c[1]=cos(theta)*.45+theta/(1+theta)-cos(.33*theta+.3)-1;
      theta+=.03;
      for(i=0;i<height;i++)
	{
	  for(j=0;j<width;j++)
	    {
	      x[0]=(j-width/2)*3.0/width;
	      x[1]=(i-height/2)*3.0/height;
	      f(x,c,z);
	      d=sqrt(z[0]*z[0]+z[1]*z[1]);
	      if(d<1) putchar(d*256);
	      else putchar(255);

	      if(d<1) putchar(0);
	      else if(d<3) putchar((d-1)*128);
	      else putchar(255);

	      if(d<3) putchar(0);
	      else if(d<19) putchar((d-3)*16);
	      else putchar(255);
	    }
	}
    }
  return 0;
}


