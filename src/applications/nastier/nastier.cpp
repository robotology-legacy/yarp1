#include <stdio.h>

#include "conf/tx_data.h"

#include "YARPThread.h"
#include "YARPPort.h"
#include "YARPTime.h"


YARPInputPortOf<StereoPosData> in_pos;
//YARPOutputPortOf<Data> out_data;

int main()
{
  in_pos.Register("/nastier/in");
  //  out_data.Register("/cckemp/out");

  while (1)
    {
      in_pos.Read();
      StereoPosData& pos = in_pos.Content();
      printf("The position as of this time is %4g,%4g  %4g,%4g, %d :", 
	     pos.xl, pos.yl,
	     pos.xr, pos.yr, pos.valid);
      double xt, yt;
      xt = pos.xl - 63;
      yt = pos.yl - 63;
      printf("%4g,%4g\n", xt, yt);
    }

  return 0;
}


