
Make sure to source the following before compiling:

. /usr/lib/GNUstep/System/Makefiles/GNUstep.sh

example of how to run:

  ./obj/tube emergency.txt emergency.wav
  make && artsdsp -s -m ./obj/tube -r emergency.txt ignore.wav


gnuspeech website: http://savannah.gnu.org/projects/gnuspeech

debian packages: gnustep, gobjc, gnustep-devel
also: fftw3-dev

compile and install extensions in:
  current/Applications/Monet/Extensions/
(from CVS)

compile in:
  current/Tools/softwareTRM

tube executable is:
  current/Tools/softwareTRM/obj/tube


Manual for monet is at:

  http://pages.cpsc.ucalgary.ca/~hill/papers/monman/index.html

