/*
  Mandel: Mandelbrot set viewer, a test program for grap.
  Copyright (C) 1999 Sebastien Loisel

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* example invokation: mandel 300 400 | grap 300 400 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>


void f(double x[2], double c[2])
{
  double foo[2];

  foo[0]=x[0]*x[0]-x[1]*x[1]+c[0];
  foo[1]=2*x[0]*x[1]+c[1];

  x[0]=foo[0]; x[1]=foo[1];
}

void makemandel(int w, int h, int iter,
		double x, double y, double zoom)
{
  int i,j,k;
  double z[2],c[2];

  for(i=0;i<h;i++)
    for(j=0;j<w;j++)
      {
	z[0]=z[1]=0;
	c[1]=(i-h/2)*zoom+y;
	c[0]=(j-w/2)*zoom+x;
	for(k=0;k<iter;k++)
	  {
	    f(z,c);
	    if(z[0]*z[0]+z[1]*z[1]>4) break;
	  }
	if(k==iter)
	  {
	    putchar(0); putchar(0); putchar(255);
	  }
	else
	  {
	    if(k>iter/3) putchar(255);
	    else putchar(k*255*3/iter);
	    if(k>iter/2) putchar(0);
	    else putchar((iter/2-k)*255*2/iter);
	    putchar(k*255/iter);
	  }
      }
}

int main(int argc, char *argv[])
{
  int w,h,iter;
  int i,j;
  double x,y,zoom;
  char s[256];

  w=h=iter=100;
  if(argc>=2)
    {
      w=atoi(argv[1]);
      if(argc>=3)
	h=atoi(argv[2]);
    }
  if(w<=0 || h<=0 || argc>=4)
    {
      puts("Usage: mandel [<width> [<height>]]");
      return;
    }

  x=y=0;
  zoom=4.0/w;
  while(1)
    {
      makemandel(w,h,iter,x,y,zoom);
      fflush(stdout);
      fprintf(stderr,
	      "x=%f, y=%f, zoom=%f, increment=%f, iter=%i\n"
	      "+: multiply zoom by 1.5\n"
	      "-: divide zoom by 1.5\n"
	      "-<number>: set zoom to number\n"
	      "s: x+=increment\n"
	      "a: x-=increment\n"
	      "w: y-=increment\n"
	      "z: y+=increment\n"
	      "q: quit\n"
	      "i<number>: set number of iterations\n",
	     x,y,zoom,zoom*50,iter);
      gets(s);
      switch(s[0])
	{
	case '+': zoom*=1.5; break;
	case '-':
	  if(s[1]==0)
	    zoom/=1.5;
	  else
	    zoom=atof(s+1);
	  break;
	case 's': x+=zoom*50; break;
	case 'a': x-=zoom*50; break;
	case 'w': y-=zoom*50; break;
	case 'z': y+=zoom*50; break;
	case 'q': return 0;
	case 'i': iter=atoi(s+1); break;
	default: fprintf(stderr,"Command unrecognized: '%s'\n",s);
	}
    }
  return 0;
}

