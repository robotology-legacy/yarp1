
from: http://savannah.gnu.org/projects/gnuspeech

Requires debian packages:
  gnustep gobjc gnustep-devel fftw-dev

Make sure to source the following before compiling anything:

. /usr/lib/GNUstep/System/Makefiles/GNUstep.sh

compile and install extensions in:
  Applications/Monet/Extensions/
using:
  make
  make install

compile in:
  Tools/softwareTRM
using:
  make

tube executable is:
  Tools/softwareTRM/obj/tube

see Tools/softwareTRM/README.TXT for information on how to run
