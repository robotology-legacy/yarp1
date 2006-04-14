yarp-connect /mirrorCollector/o:img0 !/mirrorGrasp/i:img0
yarp-connect /mirrorCollector/o:img1 !/mirrorGrasp/i:img1
yarp-connect /mirrorCollector/o:str  !/mirrorGrasp/i:str
yarp-connect /mirrorGrasp/o:int      !/mirrorCollector/i:int
yarp-connect /mirrorCollector/o:int  !/mirrorGrasp/i:int

yarp-connect /mirrorCollector/o:img0 /mirrorGrasp/i:img0
yarp-connect /mirrorCollector/o:img1 /mirrorGrasp/i:img1
yarp-connect /mirrorCollector/o:str /mirrorGrasp/i:str
yarp-connect /mirrorGrasp/o:int /mirrorCollector/i:int
yarp-connect /mirrorCollector/o:int /mirrorGrasp/i:int
