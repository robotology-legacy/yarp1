var WshShell = WScript.CreateObject("WScript.Shell");

//WshShell.Run ("on -d -n leto \"framegrab.exe -w 128 -h 128 -b 0 -name cart -net Net1 -o 4\"", 7, true);
//WshShell.Run ("on -d -n leto \"camview.exe -name pfview5/i:img -x 1 -y 1 -net Net1 -p 100\"", 7, true);

WshShell.Run ("on -d -n leto \"grabber.exe --w 256 --h 256 --b 0 --name /cart --net Net1 --o 4 --shmem --protocol tcp\"", 7, true);
WshShell.Run ("on -d -n leto \"camview.exe --name /pfview5/i:img --w 256 --h 256 --x 1 --y 1 --net Net1 --p 100\"", 7, true);





//WshShell.Run ("on -d -n leto \"framegrab.exe -w 10 -h 10 -b 0 -name cart -n Net1 -o 4\"", 7, true);
//WshShell.Run ("on -d -n leto \"camview.exe -name pfview5/i:img -w 10 -h 10 -x 1 -y 1 -net Net1 -p 100\"", 7, true);


//WshShell.Run ("on -d -n leto \"camview.exe -name pfview2/i:img -net Net1 -p 100\"", 7, true);
