// $ID$
YARP device driver, tree structure.

For each board a folder:
./BOARD_NAME/
containing:
OS/ -> wrapper code (.cpp and .h)
OS/dd_orig  à the original device drivers shipped with the board (.h and .lib)

“OS” may be winnt, qnx, qnx6 or linux.

--
NOTES:

- Manually copy original device driver .h to YARP_ROOT/include/sys (install_orig_dd.bat will do it for you)

- Win32
alldrivers.dsw is the visual C++ workspace which compiles and builds all the device drivers on win32 (YARP_ROOT/lib/winnt/alldrivers.lib).
The post-build process copies the include files to YARP_ROOT/include.
