@echo installing ipl...

copy .\include\*.h ..\..\..\include\sys
copy .\lib\*.lib ..\..\..\lib\winnt
copy .\bin\*.dll ..\..\..\bin\winnt

@echo done!