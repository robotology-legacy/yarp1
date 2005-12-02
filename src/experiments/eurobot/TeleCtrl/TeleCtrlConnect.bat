#
# this script is run by TeleCtrl.
#
# WARNING: mirrorCollector, repeater and headcontrol must be
#  up'n'running BEFORE TeleCtrl is started and this script is invoked.
#

# ------------------ try and disconnect all ports for safety
yarp-connect /mirrorCollector/o:img  !/TeleCtrlToMaster/i:img
yarp-connect /mirrorCollector/o:str  !/TeleCtrlToMaster/i:str
yarp-connect /TeleCtrlToMaster/o:int !/mirrorCollector/i:int
yarp-connect /mirrorCollector/o:int  !/TeleCtrlToMaster/i:int
yarp-connect /TeleCtrlToSlave/o:str  !/repeater/i

# ------------------ connect TeleCtrl to mirrorCollector (MASTER)
yarp-connect /mirrorCollector/o:img /TeleCtrlToMaster/i:img
yarp-connect /mirrorCollector/o:str /TeleCtrlToMaster/i:str
yarp-connect /TeleCtrlToMaster/o:int /mirrorCollector/i:int
yarp-connect /mirrorCollector/o:int /TeleCtrlToMaster/i:int

# ------------------ connect TeleCtrl to repeater (SLAVE)
yarp-connect /TeleCtrlToSlave/o:str /repeater/i

# ------------------ this is essential: ports must be fully
#                    connected *before* the program may go on
sleep 5
