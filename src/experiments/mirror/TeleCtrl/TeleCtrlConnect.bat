@echo off

yarp-connect /mirrorCollector/o:img0  !/TeleCtrlToMaster/i:img0
yarp-connect /mirrorCollector/o:img1  !/TeleCtrlToMaster/i:img1
yarp-connect /mirrorCollector/o:str  !/TeleCtrlToMaster/i:str
yarp-connect /TeleCtrlToMaster/o:int !/mirrorCollector/i:int
yarp-connect /mirrorCollector/o:int  !/TeleCtrlToMaster/i:int
yarp-connect /TeleCtrlToSlave/o:str  !/repeater/i

yarp-connect /mirrorCollector/o:img0 /TeleCtrlToMaster/i:img0
yarp-connect /mirrorCollector/o:img1 /TeleCtrlToMaster/i:img1
yarp-connect /mirrorCollector/o:str /TeleCtrlToMaster/i:str
yarp-connect /TeleCtrlToMaster/o:int /mirrorCollector/i:int
yarp-connect /mirrorCollector/o:int /TeleCtrlToMaster/i:int

yarp-connect /TeleCtrlToSlave/o:str /repeater/i

