#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int width,height;
char *picture;


void putball(float x, float y, int R, int G, int B)
{
  int i,j,k;
  int x0,x1,y0,y1;
  float a,b;

  x0=x-5; x1=x+6; y0=y-5; y1=y+6;
  if(x0<0) x0=0; if(x1>width) x1=width;
  if(y0<0) y0=0; if(y1>height) y1=height;

  for(i=x0;i<x1;i++)
    {
      for(j=y0;j<y1;j++)
	{
	  a=i-x; b=j-y; k=(i+j*width)*3;
	  if(a*a+b*b<25)
	    { picture[k]=R; picture[k+1]=G; picture[k+2]=B; }
	}
    }
}

float x[1000][2],dx[1000][2];
int rgb[1000][3],nballs;

void boing()
{
  int i;

  srand(time(0));
  picture=malloc(width*height*3);
  if(!picture)
    {
      puts("boing: could not allocate buffer");
      abort();
    }
  for(i=0;i<width*height*3;i++) picture[i]=0;
  for(i=0;i<nballs;i++)
    {
      x[i][0]=(rand()%(width-14))+7;
      x[i][1]=(rand()%(height-14))+7;
      dx[i][0]=(rand()%100)/50.0-1;
      dx[i][1]=(rand()%100)/50.0-1;
      rgb[i][0]=rand()%256;
      rgb[i][1]=rand()%256;
      rgb[i][2]=rand()%256;
    }
      
  while(1)
    {
      for(i=0;i<nballs;i++)
	{
	  if(x[i][0]<5 || x[i][0]>width-5) dx[i][0]=-dx[i][0];
	  if(x[i][1]<5 || x[i][1]>height-5) dx[i][1]=-dx[i][1];
	  x[i][0]+=dx[i][0];
	  x[i][1]+=dx[i][1];
	  putball(x[i][0],x[i][1],rgb[i][0],rgb[i][1],rgb[i][2]);
	}
	  
      fwrite(picture,width*3,height,stdout);
      fflush(stdout);
      for(i=0;i<nballs;i++)
	{
	  putball(x[i][0],x[i][1],0,0,0);
	}
    }
}

int main(int argc, char *argv[])
{
  width=height=100;
  nballs=25;
  if(argc>1)
    {
      width=atoi(argv[1]);
      if(argc>2)
	{
	  height=atoi(argv[2]);
	  if(argc>3)
	    {
	      nballs=atoi(argv[3]);
	      if(nballs>900) nballs=900;
	    }
	}
    }

  fprintf(stderr,"boing: size is %ix%i, nballs=%i\n",width,height,nballs);
  boing();
  return 0;
}

