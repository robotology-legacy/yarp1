yarp-connect /mirrorCollector/o:img !/mirrorSaver/i:img
yarp-connect /mirrorCollector/o:str !/mirrorSaver/i:str
yarp-connect /mirrorSaver/o:int !/mirrorCollector/i:int
yarp-connect /mirrorCollector/o:int !/mirrorSaver/i:int

