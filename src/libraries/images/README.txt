WINNT version: the project compiles assuming the IPL in the std path
installed in the default path Intel/plsuite/

IPL library is not linked to the image_services. It would be possible
though to include IPL stuff together with the image_services so that 
applications have to link only one lib. Note that actual IPL stuff is
dll anyway.

The "extra" directory contasins image processing objects that need other 
libraries to work correctly (VisMatrix). Files are included in the same 
project.

QNX. Added include file YARPSafeNew.h to handle allocation in a 
multithread environment with the Watcom compiler.
