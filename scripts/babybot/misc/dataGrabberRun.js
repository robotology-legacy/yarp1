var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("on -d -n leto \"datacollector.exe -neti Net0 -name datagrabber -length 50 -dir C:/temp/exp1/\"", 7, true);