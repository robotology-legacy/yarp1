// CollectorTest.cpp : Defines the entry point for the console application.
//

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <iostream>
#include <stdio.h>

#include <YARPThread.h>
#include <YARPScheduler.h>
#include <YARPImages.h>
#include <YARPImageFile.h>
#include <YARPMirrorTracker.h>
#include <YARPMirrorDataGlove.h>
#include <YARPMirrorPresSens.h>

#include "CollectorCommands.h"


YARPInputPortOf<MNumData> _data_inport;
YARPInputPortOf<YARPGenericImage> _img_inport;
YARPInputPortOf<int> _rep_inport;
YARPOutputPortOf<MCommands> _cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
int _sizeX;
int _sizeY;

void writeDataToFile(FILE* pFile, MNumData data, int i);


class mainthread : public YARPThread
{
public:
	virtual void Body (void);
};

void mainthread::Body (void)
{
	FILE *str_outfile = NULL;
	char streamFileName[255];
	int j = 0;
	MNumData str_data;
	YARPImageOf<YarpPixelBGR> str_img;
	str_img.Resize (_sizeX, _sizeY);
	str_outfile = fopen("str_data.txt","w");
	if (str_outfile == NULL)
	{
		cout << "FATAL ERROR: impossible to open output file 'str_data.txt'." << endl;
		exit(YARP_FAIL);
	}

	while (!IsTerminated())
	{
		_data_inport.Read();
		_img_inport.Read();
		j++;
		str_data = _data_inport.Content();
		writeDataToFile(str_outfile,str_data,j);
		str_img.Refer(_img_inport.Content());
		sprintf(streamFileName,"str_image%d.pgm", j);
		YARPImageFile::Write(streamFileName, str_img,YARPImageFile::FORMAT_PPM);
	}
	
	fclose(str_outfile);

	return;
}

void writeDataToFile(FILE* pFile, MNumData data, int i)
{
	fprintf(pFile,"%d ",i);
	fprintf(pFile," %f %f %f %f %f %f", data.tracker.x, data.tracker.y, data.tracker.z, data.tracker.azimuth, data.tracker.elevation, data.tracker.roll);
	fprintf(pFile," %d %d %d", data.glove.thumb[0], data.glove.thumb[1], data.glove.thumb[2]);
	fprintf(pFile," %d %d %d", data.glove.index[0], data.glove.index[1], data.glove.index[2]);
	fprintf(pFile," %d %d %d", data.glove.middle[0], data.glove.middle[1], data.glove.middle[2]);
	fprintf(pFile," %d %d %d", data.glove.ring[0], data.glove.ring[1], data.glove.ring[2]);
	fprintf(pFile," %d %d %d", data.glove.pinkie[0], data.glove.pinkie[1], data.glove.pinkie[2]);
	fprintf(pFile," %d %d %d %d %d", data.glove.abduction[0], data.glove.abduction[1], data.glove.abduction[2], data.glove.abduction[3], data.glove.abduction[4]);
	fprintf(pFile," %d", data.glove.palmArch);
	fprintf(pFile," %d %d", data.glove.wrist[0], data.glove.wrist[1]);
	fprintf(pFile," %d %d %d %d", data.pressure.channelA, data.pressure.channelB, data.pressure.channelC, data.pressure.channelD);
	fprintf(pFile, "\n");
}

int main(int argc, char* argv[])
{
// Data port registration
	cout << "Registering Data port (input)..." << endl;
	int ret = 0;	
	ret = _data_inport.Register("/test/i:str","default");
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name '/test/i:str' (see above)."  << endl;
		exit(YARP_FAIL);
	}

// Images port registration
	cout << "Registering Images port (input)..." << endl;
	ret = 0;	
	ret = _img_inport.Register("/test/i:img","default");
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name '/test/i:img' (see above)."  << endl;
		exit(YARP_FAIL);
	}

// Commands port registration
	cout << "Registering Commands port (output)..." << endl;
	ret = 0;	
	ret = _cmd_outport.Register("/test/o:int","default");
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering input port with name '/test/o:int' (see above)."  << endl;
		exit(YARP_FAIL);
	}

// Response port registration
	cout << "Registering Response port..." << endl;
	ret = 0;	
	ret = _rep_inport.Register("/test/i:int", "default");
	if (ret != YARP_OK)
	{
		cout << endl << "FATAL ERROR: problems registering output port with name '/test/i:int' (see above)."  << endl;
		exit(YARP_FAIL);
	}
	
	char c;
	int reply;
	MNumData data;
	YARPImageOf<YarpPixelBGR> img;
	_sizeX = 0;
	_sizeY = 0;
	FILE *outfile;
	int i;
	mainthread saverThread;
	char fileName[255];
		
	YARPScheduler::setHighResScheduling ();

	bool bQuit = false;
	do
	{
		cout << "1 - Connect" << endl;
		cout << "2 - Disconnect and Quit" << endl;
		cout << "3 - Get Data" << endl;
		cout << "4 - Start Streaming" << endl;
		cout << "5 - Stop Streaming" << endl;
		cout << "6 - Reset Glove" << endl;
		cout << "q - Quit" << endl;
		cout << "Command to send? ";
		cin >> c;
		switch (c)
		{
		case '1':
			cout << endl << "Connecting..";
			outfile = fopen("data.txt","w");
			if (outfile == NULL)
			{
				cout << "FATAL ERROR: impossible to open output file 'data.txt'." << endl;
				exit(YARP_FAIL);
			}
			_cmd_outport.Content() = CCMDConnect;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
			{
				_rep_inport.Read();
				_sizeX = _rep_inport.Content();
				_rep_inport.Read();
				_sizeY = _rep_inport.Content();
				img.Resize (_sizeX, _sizeY);
				cout << " Done. Image Size is " << _sizeX << " x " << _sizeY << endl;
			}
			else
				cout << " Failed." << endl;
			break;
		case '2':
			cout << endl << "Sending the Kill command..";
			fclose(outfile);
			_cmd_outport.Content() = CCMDQuit;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
			{
				bQuit = true;
				cout << " Done." << endl;
			}	
			else
				cout << " Failed." << endl;
			break;
		case '3':
			cout << endl << "Asking for data..";
			_cmd_outport.Content() = CCMDGetData;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
			{
				cout << " Receiving data..";
				_data_inport.Read();
				_img_inport.Read();
				i++;
				data = _data_inport.Content();
				writeDataToFile(outfile,data,i);
				img.Refer(_img_inport.Content());
				sprintf(fileName,"image%d.pgm", i);
				YARPImageFile::Write(fileName,img,YARPImageFile::FORMAT_PPM);
				cout << " Done." << endl;
			}	
			else
				cout << " Failed." << endl;
			break;
		case '4':
			cout << endl << "Starting streaming mode..";
			_cmd_outport.Content() = CCMDStartStreaming;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
			{
				saverThread.Begin();
				cout << " Done." << endl;
			}
			else
				cout << " Failed." << endl;
			break;
		case '5':
			cout << endl << "Stopping streaming mode..";
			_cmd_outport.Content() = CCMDStopStreaming;
			_cmd_outport.Write();
			if ( reply == CMD_ACK)
			{
				saverThread.End();
				cout << " Done." << endl;
			}
			else
				cout << " Failed." << endl;
			break;
		case '6':
			cout << endl << "Resetting the glove..";
			_cmd_outport.Content() = CCMDResetGlove;
			_cmd_outport.Write();
			_rep_inport.Read();
			reply = _rep_inport.Content();
			if ( reply == CMD_ACK)
				cout << " Done." << endl;
			else
				cout << " Failed." << endl;
			break;
		case 'q':
			bQuit = true;
		}
	} while (!bQuit);
	
	return 0;
}
