#ifndef FACECONTENT_INC
#define FACECONTENT_INC

#include "YARPImage.h"
#include "YARPImagePort.h"
#include "YARPPortContent.h"
#include <conf/tx_data.h>

class FaceContent : public YARPPortContent
{
public:
  YARPImagePortContent img;
  YARPPortContentOf<FiveBoxesInARow> boxes;
  
  YARPGenericImage& GetImage() { return img; }
  FiveBoxesInARow& GetBoxes()  { return boxes.Content(); }
  
  virtual int Read(YARPPortReader& reader)
    {
      img.Read(reader);
      return boxes.Read(reader);
    }
  
  virtual int Write(YARPPortWriter& writer)
    {
      img.Write(writer);
      return boxes.Write(writer);
    }
  
  virtual int Recycle()
    {
      img.Recycle();
      boxes.Recycle();
      return 0;
    }
};

#endif
