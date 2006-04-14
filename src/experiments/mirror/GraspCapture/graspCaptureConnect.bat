yarp-connect /fakeCollector/o:img0 !/mirrorGrasp/i:img0
yarp-connect /fakeCollector/o:img1 !/mirrorGrasp/i:img1
yarp-connect /fakeCollector/o:str  !/mirrorGrasp/i:str
yarp-connect /mirrorGrasp/o:int      !/fakeCollector/i:int
yarp-connect /fakeCollector/o:int  !/mirrorGrasp/i:int

yarp-connect /fakeCollector/o:img0 /mirrorGrasp/i:img0
yarp-connect /fakeCollector/o:img1 /mirrorGrasp/i:img1
yarp-connect /fakeCollector/o:str /mirrorGrasp/i:str
yarp-connect /mirrorGrasp/o:int /fakeCollector/i:int
yarp-connect /fakeCollector/o:int /mirrorGrasp/i:int
