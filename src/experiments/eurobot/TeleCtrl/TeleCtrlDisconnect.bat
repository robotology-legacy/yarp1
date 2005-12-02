#
# this script is run by TeleCtrl.
#

# ------------------ disconnect all ports
yarp-connect /mirrorCollector/o:img  !/TeleCtrlToMaster/i:img
yarp-connect /mirrorCollector/o:str  !/TeleCtrlToMaster/i:str
yarp-connect /TeleCtrlToMaster/o:int !/mirrorCollector/i:int
yarp-connect /mirrorCollector/o:int  !/TeleCtrlToMaster/i:int
yarp-connect /TeleCtrlToSlave/o:str  !/repeater/i
