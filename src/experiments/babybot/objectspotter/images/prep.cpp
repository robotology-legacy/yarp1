
#include <stdio.h>

#include <yarp/YARPImageFile.h>
#include <yarp/YARPImageDraw.h>

#define y1 icantbelievetheybrokey1

int s1 = 0;
int x1 = 128;
int y1 = 128;
int r1 = 30;
int s2 = 0;
int x2 = 128;
int y2 = 128;
int r2 = 25;
int ct = 0;
double ltheta = 0;


void render(int grasp) {
  YARPImageOf<YarpPixelRGB> img;

  img.Resize(256,256);

  img.Zero();
  double dx = cos(ltheta);
  double dy = sin(ltheta);
  IMGFOR(img,x,y) {
    img(x,y).r = (int)(100+90*sin(0.05*(dx*x+dy*y)));
  }
  ltheta += 0.05;
  YarpPixelRGB pix1(0,255,0);
  YarpPixelRGB pix2(0,255,255);

  if (s1) {
    AddCircle(img,pix1,x1,y1,r1);
  }
  if (s2) {
    for (int xx=x2-r2; xx<=x2+r2; xx++) {
      for (int yy=y2-r2*0.5; yy<=y2+r2*0.5; yy++) {
	img(xx,yy) = pix2;
      }
    }
  }

  char buf[256];
  sprintf(buf,"objspot_%06d.ppm",ct);
  ct++;
  YARPImageFile::Write(buf,img);
  printf("images/%s %d\n", buf, grasp);
}

int main() {
  render(0);
  s1 = 1;
  x1 = 128;
  y1 = 128;
  render(1);
  x1 = 200;
  y1 = 128;
  render(0);
  x1 = 200;
  y1 = 200;
  render(0);
  x1 = 50;
  y1 = 250;
  r1 = 90;
  render(0);
  s1 = 1;
  s2 = 1;
  render(0);
  render(1);
  render(0);
  x1 = 128;
  y1 = 128;
  r1 = 30;
  x2 = 128;
  y2 = 200;
  r1 = 40;
  render(0);
  r1 = 12;
  render(0);
  s1 = 0;
  render(0);
  
  return 0;
}

