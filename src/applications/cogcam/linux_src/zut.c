/*
  Zut 1.3: Zuthical Pipe, a zuthical back-end.
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

/* You pipe the output of your program into this and it'll put it in a
   window. Format is simple. In the "client" program, you would say
   putchar(0); putchar(0); putchar(255); and that would stand
   for red=0, green=0 and blue=255. You output all the pixels like so.
   The default screen size is 100x100, meaning that you'll need to put out
   10000 pixels, that's 30000 bytes. Run zut -h to get all the possible
   parameters. */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>


#include <gpl.h>

int pf_hack_24 = 0;

Display *display;
int screen,depth,width,height,xshmevent=-1,xshm=0,buffered=0,xdpm=0;
Window rootwin,win;
GC gc;
XImage *image=0;
int redshift,greenshift,blueshift,nodraw=0,verbose=0;

int init(int w, int h)
{
  char *window_name = "Zut";
  char *icon_name = "Zut";
  XSizeHints size_hints;
  Window rootwin;
  
  display = XOpenDisplay(NULL);
  
  if (display == NULL)
    {
      printf("zut: Failed to open display\n");
      return 1;
    }
  
  screen = DefaultScreen(display);
  depth = DefaultDepth(display, screen);
  rootwin = RootWindow(display, screen);
  gc=DefaultGC(display,screen);
  win = XCreateSimpleWindow(display, rootwin, 10, 10, w, h, 5,
			    BlackPixel(display, screen), BlackPixel(display, screen));
  
  size_hints.flags = PSize | PMinSize | PMaxSize;
  size_hints.min_width = w;
  size_hints.max_width = w;
  size_hints.min_height = h;
  size_hints.max_height = h;
  
  XSetStandardProperties(display, win, window_name, icon_name, None,
			 0, 0, &size_hints); 
  
  XSelectInput(display, win, ExposureMask|StructureNotifyMask);
  XMapWindow(display, win);
  if(verbose)
    printf("zut: %ix%i window opened\n",w,h);
  return 0;
}

void eventloop()
{
  XEvent xev;
  int num_events;

  while(1)
    {
      XFlush(display);
      num_events = XPending(display);
      if(num_events==0) return;
      while((num_events != 0))
	{
	  num_events--;
	  XNextEvent(display, &xev);
	  if(xev.type==DestroyNotify)
	    exit(0);
	  if(xev.type==Expose && image!=0)
	    {
	      if(xshm)
		XShmPutImage(display,win,gc,image,0,0,0,0,image->width,
			     image->height,0);
	      else if(!xdpm)
		XPutImage(display,win,gc,image,0,0,0,0,image->width,image->height);
	    }
	}
    }
}


int compose(int i, int shift)
{ if(shift<0) return (i>>(-shift)); else return (i<<shift); }

void putlinegeneric8(unsigned char *buf, int y, int y2)
{
  int j,a;
  unsigned char *dest;

  dest=(unsigned char *)(image->data+y*image->bytes_per_line);
  a=0;
  while(y<y2)
    {
      for(j=0;j<width;j++,a+=3)
	{
	  dest[j]=(compose(buf[a],redshift)&image->red_mask)+
	    (compose(buf[a+1],blueshift)&image->blue_mask)+
	    (compose(buf[a+2],greenshift)&image->green_mask);
	}
      dest+=image->bytes_per_line; y++;
    }
}
void putlinegeneric16(unsigned char *buf, int y, int y2)
{
  int j,a;
  unsigned short *dest;

  a=0;
  while(y<y2)
    {
      dest=(unsigned short *)(image->data+y*image->bytes_per_line);
      y++;

      for(j=0;j<width;j++,a+=3)
	{
	  dest[j]=(compose(buf[a],redshift)&image->red_mask)+
	    (compose(buf[a+1],blueshift)&image->blue_mask)+
	    (compose(buf[a+2],greenshift)&image->green_mask);
	}
    }
}
void putline16RGB565(unsigned char *buf, int y, int y2)
{
  int j,a;
  unsigned short *dest;
  a=0;
  while(y<y2)
    {
      dest=(unsigned short *)(image->data+y*image->bytes_per_line);
      y++;
      for(j=0;j<width;j++,a+=3)
	{
	  dest[j]=((((buf[a]<<8)|(buf[a+2]>>3))&0xf81f)|((buf[a+1]<<3)&0x7e0));
	}
    }
}
void putlinegeneric32(unsigned char *buf, int y, int y2)
{
  int j,a;
  unsigned int *dest;

  a=0;
  while(y<y2)
    {
      dest=(unsigned int *)(image->data+y*image->bytes_per_line);
      y++;
      for(j=0;j<width;j++,a+=3)
	{
	  dest[j]=(compose(buf[a],redshift)&image->red_mask)+
	    (compose(buf[a+1],blueshift)&image->blue_mask)+
	    (compose(buf[a+2],greenshift)&image->green_mask);
	}
    }
}

void putline32RGB888(unsigned char *buf, int y, int y2)
{
  int j,a;
  unsigned int *dest;

  a=0;
  while(y<y2)
    {
      dest=(unsigned int *)(image->data+y*image->bytes_per_line);
      y++;
      
      for(j=0;j<width;j++,a+=3)
	{
	  dest[j]=((buf[a]<<16)|(buf[a+1]<<8)|(buf[a+2]));
	}
    }
}


int readsome(unsigned char *buf, int count)
{
  fd_set fd;
  int pos,size;

  pos=0;
  while(count>0)
    {
      FD_ZERO(&fd);
      FD_SET(STDIN_FILENO,&fd);
      FD_SET(ConnectionNumber(display),&fd);
      select(ConnectionNumber(display)+1,&fd,0,0,0);
      if(FD_ISSET(ConnectionNumber(display),&fd))
	{
	  eventloop();
	}
      if(FD_ISSET(STDIN_FILENO,&fd))
	{
	  size=read(STDIN_FILENO,buf+pos,count);
	  //printf("readsome %d %d %d\n",size,count,pos);
	  if(size==0) return 1;
	  count-=size; pos+=size;
	}
    }
  //printf("readsome done\n");
  return 0;
}
int fps=0;
void coreloop()
{
  int i,k;
  unsigned char *buf;
  void (*f)(unsigned char *buf, int y, int y2);
  int tlast, fcount;

  if(buffered)
    buf=malloc(height*width*3);
  else
    buf=malloc(width*3);
  if(!buf)
    {
      puts("zut: could not allocate input buffer");
      return;
    }
  switch(image->bits_per_pixel)
    {
    case 8: f=&putlinegeneric8; 
      if(verbose) puts("zut: putlinegeneric8"); break;
    case 16:
      if(image->red_mask==0xf800 &&
	 image->green_mask==0x7e0 &&
	 image->blue_mask==0x1f)
	{
	  f=&putline16RGB565;
	  if(verbose) puts("zut: putline16RGB565 (fastpath)");
	}
      else
	{
	  f=&putlinegeneric16;
	  if(verbose) puts("zut: putlinegeneric16");
	}
      break;
    case 32:
      if(image->red_mask==0xff0000 &&
	 image->green_mask==0xff00 &&
	 image->blue_mask==0xff)
	{
	  f=&putline32RGB888;
	  if(verbose) puts("zut: putline32RGB888 (fastpath)");
	}
      else
	{
	  f=&putlinegeneric32;
	  if(verbose) puts("zut: putlinegeneric32");
	}
      break;
    }

  eventloop();
  fcntl(STDIN_FILENO,F_GETFL,&k);
  k|=O_NONBLOCK;
  fcntl(STDIN_FILENO,F_SETFL,k);
  tlast=time(0); fcount=0;
  while(1)
    {
      if(buffered)
	{
	  if(readsome(buf,width*height*3)) return;
	  if(!nodraw)
	    {
	      f(buf,0,height);
	      if(xshm)
		{
		  XShmPutImage(display,win,gc,image,0,0,0,0,image->width,
			       image->height,0);
		}
	      else
		XPutImage(display,win,gc,image,0,0,0,0,
			  image->width,image->height);
	    }
	}
      else
	{
	  for(i=0;i<height;i++)
	    {
	      if(readsome(buf,width*3)) return;
	      f(buf,i,i+1);
	      if(!nodraw)
		{
		  if(xshm)
		    {
		      XShmPutImage(display,win,gc,image,0,i,0,i,image->width,
				   1,0);
		    }
		  else
		    XPutImage(display,win,gc,image,0,i,0,i,
			      image->width,image->height);
		}
	      eventloop();
	    }
	}
      eventloop();
      if(fps)
	{
	  k=time(0); fcount++;
	  if(k>tlast)
	    {
	      printf("zut: fps count: %i\n",fcount/(k-tlast));
	      fcount=0; tlast=k;
	    }
	}
    }
}

void mainloop_noshm()
{
  int i,j;
  XColor color;
  if(verbose) puts("zut: mainloop_noshm()");
  while(1)
    {
      for(i=0;i<height;i++)
	for(j=0;j<width;j++)
	  {
	    eventloop();
	    color.red=getchar()*256;
	    if(feof(stdin)) return;
	    color.green=getchar()*256;
	    if(feof(stdin)) return;
	    color.blue=getchar()*256;
	    if(feof(stdin)) return;
	    XAllocColor(display,DefaultColormap(display,screen),&color);
	    XSetForeground(display,gc,color.pixel);
	    XDrawPoint(display,win,gc,j,i);
	  }
    }
}


int initimage()
{
  int i;
  unsigned int tmp;

  if(!image)
    {
      puts("zut: XCreateImage failed");
      return 1;
    }

  if(image->red_mask==0 ||
     image->green_mask==0 ||
     image->blue_mask==0)
    {
      printf("zut: could not get an RGB pixmap");
      return 1;
    }
  i=image->red_mask; redshift=-8;
  while(i) { i/=2; redshift++; }
  i=image->green_mask; greenshift=-8;
  while(i) { i/=2; greenshift++; }
  i=image->blue_mask; blueshift=-8;
  while(i) { i/=2; blueshift++; }
  if (pf_hack_24)
    {
      redshift += 8;
      greenshift += 8;
      blueshift += 8;

      tmp = blueshift;
      blueshift = redshift;
      redshift = tmp;

      tmp = blueshift;
      blueshift = greenshift;
      greenshift = tmp;

      image->red_mask*=256;
      image->green_mask*=256;
      image->blue_mask*=256;

      tmp = image->blue_mask;
      image->blue_mask = image->red_mask;
      image->red_mask = tmp;

      tmp = image->blue_mask;
      image->blue_mask = image->green_mask;
      image->green_mask = tmp;
    }
  if(verbose)
    printf("zut: RGB masks are (%X,%X,%X), shifts are (%i,%i,%i)\n",
	   image->red_mask,image->green_mask,image->blue_mask,
	   redshift,greenshift,blueshift);
  if((image->bits_per_pixel!=16) && (image->bits_per_pixel!=32) &&
     (image->bits_per_pixel!=8))
    {
      printf("zut: image has %i bits per pixel, only supported values are "
	     "8, 16 and 32\n",image->bits_per_pixel);
      return 1;
    }
  return 0;
}

void mainloop_shm()
{
  XShmSegmentInfo shminfo;

  if(verbose)
    puts("zut: mainloop_shm()");

  image=XShmCreateImage(display,DefaultVisual(display,screen),
			DefaultDepth(display,screen),ZPixmap,
			NULL,&shminfo,width,height);
  if(initimage(image))
    return;

  shminfo.shmid=shmget(IPC_PRIVATE,image->bytes_per_line*image->height,
		       IPC_CREAT|0777);
  if(shminfo.shmid<0)
    {
      puts("zut: shmget failed");
      perror("zut");
      return;
    }
  else
    {
      if(verbose)
	printf("zut: shmget returned shmid=%i\n",shminfo.shmid);
    }

  shminfo.shmaddr = image->data = shmat(shminfo.shmid, 0, 0);
  if(shminfo.shmaddr == (char *)-1)
    {
      puts("zut: shmat failed");
      return;
    }
  shminfo.readOnly = 0;
  XShmAttach(display, &shminfo);
  if(shmctl(shminfo.shmid,IPC_RMID,0))
    {
      printf("zut: error in shmctl; you may need to delete shared memory\n"
	     "zut: resource %i manually after exit (see ipcrm(8))\n",
	     shminfo.shmid);
      perror("zut: error message was");
    }
 
  coreloop(&putlinegeneric16);
}

void mainloop_buffered()
{
  char *buf;
  int bpl,nbits;

  if(verbose)
    puts("zut: mainloop_buffered()");

  nbits=DefaultDepth(display,screen);
  if(nbits!=8 && nbits!=16 && nbits!=32)
    {
      printf("zut: Error: bits_per_rgb is %i, only 8, 16, or 32 is"
	     " supported (24 partial).\n",
	     nbits);
      if (nbits!=24)
	{
	  return;
	}
    }

  pf_hack_24 = 0;
  if (nbits==24)
    {
      nbits = 32;
      pf_hack_24 = 1;
    }

  bpl=width*nbits/8;
  if(nbits==16 && (bpl&1)) bpl++;
  if(nbits==32 && (bpl&3)) bpl+=4-(bpl&3);
  
  buf=malloc(height*bpl);
  image=XCreateImage(display,DefaultVisual(display,screen),
		     DefaultDepth(display,screen),ZPixmap,
		     0,buf,width,height,nbits,bpl);
  if(initimage(image)) return;
  coreloop(&putlinegeneric16);
}

void usage()
{
  puts("Zut! A simple graphical back-end.\n"
       "\n"
       "Usage: zut [<width> [<height>]] [options...]\n"
       "Options include:\n"
       "-x    Forces into XDrawPoint mode, try this if it's giving you\n"
       "      weird error messages after saying \"Found MIT-SHM\"; SLOW!\n"
       "      You can't use -b or -f with this, in fact, it's not even\n"
       "      line-buffered and the window doesn't know to redraw itself.\n"
       "-i    Infinite loops instead of exiting, good when you want to\n"
       "      keep the last frame on screen.\n"
       "-h    This help screen.\n"
       "-p    Input is a .ppm file, warning: must be 8 bit.\n"
       "-b    Screen-buffered. Default is line-buffered. Use this if you're\n"
       "      giving zut many frames quickly and you want faster, less jerky\n"
       "      animation.\n"
       "-n    Do not use MIT-SHM, use standard XImages instead.\n"
       "-v    Verbose mode, prints a lot of information, useful to see\n"
       "      if you're getting what you think you're getting.\n"
       "-f    Print frames per second (fps); error is +/-1 fps.\n"
       "-l    Print the license (GPL) for this program.\n"
       "-z    Do everything normally but never refresh the screen unless\n"
       "      an expose event occurs\n"
       "Options may be clustered like so: zut -vf\n"
       "Default width and height is 128.\n"
       "Bug: -p will puke if the header has a line with 898 characters "
       "or more.\n");
}

void ppmgetline(char s[])
{
  do
    {
      fgets(s,900,stdin);
    }
  while(s[0]=='#');
}

int main(int argc, char *argv[])
{
  int ignore, major, minor;
  Bool pixmaps;
  int noshm=0,sleeploop=0,ppm=0;
  int i,j,k;
  char s[1000];
  width=height=128; j=0;
  puts("Zut! 1.3, Copyright 1999 by Sebastien Loisel\n"
       "This program is distributed under the terms of the "
       "General Public License.\n"
       "For details on the license, please run zut -l or see "
       "the file gpl.txt.\n");
  for(i=1;i<argc;i++)
    {
      if(argv[i][0]!='-')
	{
	  if(j==0) width=atoi(argv[i]);
	  else if(j==1) height=atoi(argv[i]);
	  else { puts("zut: arguments are fubar"); usage(); return -1; }
	  j++;
	}
      else
	{
	  k=1;
	  do
	    {
	      switch(argv[i][k])
		{
		case 'x': xdpm=1; break;
		case 'n': noshm=1; break;
		case 'i': sleeploop=1; break;
		case 'p': ppm=1; break;
		case '-':
		case 'h': usage(); return 0;
		case 'f': fps=1; break;
		case 'b': buffered=1; break;
		case 'v': verbose=1; break;
		case 'z': nodraw=1; break;
		case 'l': puts(gpl); return 0;
		default:
		  printf("zut: Unrecognized argument '%s'; run zut -h "
			 "for usage\n\n",argv[i]);
		  usage();
		  return -1;
		}
	      k++;
	    }
	  while(argv[i][k]);
	}
    }
  if(ppm)
    {
      ppmgetline(s);
      ppmgetline(s);
      sscanf(s,"%i %i",&width,&height);
      ppmgetline(s);
      sscanf(s,"%i",&i);
      if(i!=255)
	{
	  puts("zut: error, ppm file must be 8 bit");
	  return -1;
	}
      if(verbose)
	printf("zut: displaying a %ix%i ppm file\n",width,height);
    }
  if(init(width,height))
    {
      puts("zut: Could not initialize window; run zut -h for usage");
      return -1;
    }
  if(!xdpm)
    {
      if(noshm==0)
	{
	  if(verbose)
	    puts("zut: looking for MIT-SHM");
	  if( XQueryExtension(display, "MIT-SHM", &ignore, &ignore, &ignore) )
	    {
	      if(verbose)
		puts("zut: Found MIT-SHM; looking for version number");
	      if(XShmQueryVersion(display, &major, &minor, &pixmaps) == True)
		{
		  if(verbose)
		    printf("zut: XShm extention version "
			   "%d.%d %s shared pixmaps\n",
			 major, minor, (pixmaps==True) ? "with" : "without");
		  xshm=1;
		  xshmevent=XShmGetEventBase(display)+ShmCompletion;
		} 
	    }
	  else
	    {
	      if(verbose)
		puts("zut: MIT-SHM not found");
	    }
	}
      else
	{
	  if(verbose)
	    puts("zut: MIT-SHM disabled");
	}
    }
  else
    {
      if(verbose)
	puts("zut: XDrawPoint mode");
    }
  if(buffered)
    {
      if(verbose)
	puts("zut: screen-buffered mode");
    }
  if(verbose && nodraw)
    puts("zut: nodraw mode");
  if(xshm)
    mainloop_shm();
  else if(!xdpm)
    mainloop_buffered();
  else
    mainloop_noshm();
  if(sleeploop)
    {
      while(1) sleep(10);
    }
  return 0;
}



