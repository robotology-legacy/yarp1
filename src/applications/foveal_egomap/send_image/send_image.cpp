#include "YARPImage.h"
#include "YARPPort.h"
#include "YARPImagePortContent.h"
#include "YARPImageFile.h"
#include "YARPTime.h"
#include "YARPThread.h"

YARPOutputPortOf<YARPGenericImage> out_img;

void main(int argc, char *argv[])
{
  char buf[256];
  sprintf(buf,"/send_image/%ld", (long int)YARPTime::GetTimeAsSeconds());
  out_img.Register(buf);
  printf("REGISTER DONE\n");

  int count = argc-1;
  char **arg = argv+1;
  
  int connected = 0;
  while (count>0 && !connected)
    {
      if (arg[0][0] == '+')
	{
	  printf("Calling connect...\n");
	  out_img.Connect(&arg[0][1]);
	  printf("... connect called\n");
	  arg++;
	  count--;
	}
      else
	{
	  connected = 1;
	}
    }

  while (count>0)
    {
      printf("Sending %s\n", arg[0]);
      YARPImageOf<YarpPixelBGR> img;
      YARPImageFile::Read(arg[0],img);
      out_img.Content().PeerCopy(img);
      printf("sending %d\n", out_img.IsSending());
      out_img.Write();
      printf("sending %d\n", out_img.IsSending());
      out_img.FinishSend();
      printf("sending %d\n", out_img.IsSending());
      arg++;
      count--;
    }

  //YARPTime::DelayInSeconds(1);
  
  //YARPThread::PrepareForDeath();
}
