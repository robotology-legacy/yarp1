rem compiling special libraries
echo compiling special libraries...
cd .\valuecan\winnt\dd_orig\lib
nmake /f mCAN.mak CFG="mCAN - Win32 Release"
cd ..\..\..\..

rem install header files
echo installing .h files...
copy .\nidaq\winnt\dd_orig\include\*.h ..\..\..\include\sys
copy .\mei\winnt\dd_orig\include\*.h ..\..\..\include\sys
copy .\galil\winnt\dd_orig\include\*.h ..\..\..\include\sys
copy .\picolo\winnt\dd_orig\include\*.h ..\..\..\include\sys
copy .\jr3\winnt\dd_orig\include\*.h ..\..\..\include\sys
copy .\valuecan\winnt\dd_orig\include\*.h ..\..\..\include\sys

rem install .lib files...
echo installing .lib files...
copy .\nidaq\winnt\dd_orig\lib\*.lib ..\..\..\lib\winnt
copy .\mei\winnt\dd_orig\lib\*.lib ..\..\..\lib\winnt
copy .\galil\winnt\dd_orig\lib\*.lib ..\..\..\lib\winnt
copy .\picolo\winnt\dd_orig\lib\*.lib ..\..\..\lib\winnt
copy .\jr3\winnt\dd_orig\lib\*.lib ..\..\..\lib\winnt
copy .\valuecan\winnt\dd_orig\lib\*.lib ..\..\..\lib\winnt

rem install .dll files...
echo installing dlls
copy .\nidaq\winnt\dd_orig\bin\*.dll ..\..\..\bin\winnt
copy .\mei\winnt\dd_orig\bin\*.dll ..\..\..\bin\winnt
copy .\galil\winnt\dd_orig\bin\*.dll ..\..\..\bin\winnt
copy .\picolo\winnt\dd_orig\bin\*.dll ..\..\..\bin\winnt
copy .\valuecan\winnt\dd_orig\bin\*.dll ..\..\..\bin\winnt

echo done !

