#include <iostream.h>

#include "YARPImage.h"
#include "YARPImagePort.h"
#include "YARPTime.h"
#include "CRLFaceFinderTool.h"

#include <conf/tx_data.h>

#include "FaceContent.h"

int squashed_input = 0;

#define ATTN_HEIGHT 128
#define ATTN_WIDTH  128

#ifndef for
#define for if (1) for
#endif


YARPInputPortOf<YARPGenericImage> in_img;
YARPOutputPortOf<YARPGenericImage> out_img;

YARPInputPortOf<FiveBoxesInARow> in_data;
YARPOutputPortOf<FiveBoxesInARow> out_data;

YARPBasicOutputPort<FaceContent> out_mixed;

#define DEFAULT_NAME  "/yarp/face"

int main(int argc, const char *argv[])
{
  const char *name = DEFAULT_NAME;
  char buf[256];
  if (argc>1)
    {
      name = argv[1];
    }
  sprintf(buf, "%s/i:img", name);
  in_img.Register(buf);
  sprintf(buf, "%s/o:img", name);
  out_img.Register(buf);
  sprintf(buf, "%s/o:box", name);
  out_data.Register(buf);
  sprintf(buf, "%s/o:mix", name);
  out_mixed.Register(buf);
  YARPTime::DelayInSeconds(0.05);
  CRLFaceFinderTool face_finder;
  YARPImageOf<YarpPixelMono> mwork;
  YARPImageOf<YarpPixelRGB> cwork;
  YARPImageOf<YarpPixelMono> unsquashed_image;
  
  while(1)
    {
      in_img.Read();
      mwork.CastCopy(in_img.Content());
      cwork.CastCopy(in_img.Content());

      float ratio = 4.0/3.0;
      if (squashed_input)
	{
	  unsquashed_image.ScaledCopy(mwork,ratio*mwork.GetWidth(),mwork.GetHeight());
	  face_finder.Apply(unsquashed_image);
	  for (int k=0; k<face_finder.size(); k++)
	    {
	      face_finder(k).tlx /= ratio;
	      face_finder(k).brx /= ratio;
	    }
	}
      else
	{
	  face_finder.Apply(mwork);
	}
      
//      cout << "Found " << face_finder.size() << " face(s)" << endl;
      
      FiveBoxesInARow& boxes = out_data.Content();
      FiveBoxesInARow& boxes2 = out_mixed.Content().GetBoxes();
      for (int i=0; i<FiveBoxesInARow::GetMaxBoxes(); i++)
	{
	  boxes(i).valid = false;
	  boxes2(i).valid = false;
	}

      for (int i=0; i<face_finder.size() && i<FiveBoxesInARow::GetMaxBoxes(); i++)
	{
	    {
	      CBox2Send& box = boxes(i);
	      CBox2Send& box2 = boxes2(i);
	      
	      CRLFace& face = face_finder(i);
	      box.valid = true;	  

	      // Need to change coord system to be consistent with
	      // 128x128 squashed images
	      float xfactor = ATTN_WIDTH/((float)cwork.GetWidth());
	      float yfactor = ATTN_HEIGHT/((float)cwork.GetHeight());
	      
	      box.xmin = (int)(face.tlx*xfactor+0.5);
	      box.xmax = (int)(face.brx*xfactor+0.5);
	      box.ymin = (int)(face.tly*yfactor+0.5);
	      box.ymax = (int)(face.bry*yfactor+0.5);
	      
	      box.total_sal = box.total_pixels = box.xsum = box.ysum 
		= box.centroid_x = box.centroid_y = box.cmax = box.cmin
		= box.rmax = box.rmin = 0;
	      
	      box2 = box;
	    }
	}
      face_finder.Decorate(cwork,YarpPixelRGB(0,255,0),2);
      out_mixed.Content().GetImage().ScaledCopy(cwork,ATTN_WIDTH,ATTN_HEIGHT);
      out_data.Write();
      out_mixed.Write();
      
      out_img.Content().ScaledCopy(cwork,ATTN_WIDTH,ATTN_HEIGHT);
      out_img.Write();
    }
}
