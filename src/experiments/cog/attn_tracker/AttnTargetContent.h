#ifndef ATTNTARGETCONTENT_INC
#define ATTNTARGETCONTENT_INC

#include "YARPPortContent.h"
#include <conf/tx_data.h>

class AttnTargetContent : public YARPPortContent
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
