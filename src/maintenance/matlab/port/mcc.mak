bin_target : port.dll

port.obj : D:\Users\pasa\Repository\yarp\src\maintenance\matlab\port\port.c
	cl  -c -Zp8 -G5 -W3 -DMATLAB_MEX_FILE -nologo /Foport.obj -IC:\Progra~1\Matlab6p5\extern\include -IC:\Progra~1\Matlab6p5\simulink\include /MD -O2 -Oy- -DNDEBUG D:\Users\pasa\Repository\yarp\src\maintenance\matlab\port\port.c

port.dll : port.obj
	link /out:"port.dll" /dll /export:mexFunction /MAP /LIBPATH:"C:\Progra~1\Matlab6p5\extern\lib\win32\microsoft\msvc60" libmx.lib libmex.lib libmatlb.lib libmat.lib /implib:_lib4475.x  @port_master.rsp 
	del "port.map"
	del _lib4475.x
	if exist ".res" del ".res"

