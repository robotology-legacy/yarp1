
#include "YARPImagePortContent.h"
#include "YARPPort.h"

YARPBasicInputPort<YARPImagePortContent> in_image;

void main()
{
  in_image.Register("/view");
  YARPGenericImage img[200];
  int i = 0;

  while (1)
    {
      cout << "Waiting " << endl;
      in_image.Read();
      cout << "Got something" << endl;
      img[i].Refer(in_image.Content());
      cout << "Got " << img[i].GetWidth() << "w x " 
	   << img[i].GetHeight() 	
	   << "h  (" << (long int)(&in_image.Content()) << ")" << endl;
      //i++;
    }
}

