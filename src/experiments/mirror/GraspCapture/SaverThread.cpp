// SaverThread.cpp: implementation of the CSaverThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SaverThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern int _sizeX;
extern int _sizeY;
extern YARPInputPortOf<MNumData> _data_inport;
extern YARPInputPortOf<YARPGenericImage> _img_inport;
extern YARPInputPortOf<int> _rep_inport;
extern YARPOutputPortOf<MCommands> _cmd_outport;
extern char _path[255];
extern int _nSeq;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CSaverThread::Body(void)
{
	FILE *str_outfile = NULL;
	char streamFileName[255];
	int j = 0;
	MNumData str_data;
	YARPImageOf<YarpPixelBGR> str_img;
	str_img.Resize (_sizeX, _sizeY);
	sprintf(streamFileName,"%s\\sensors.csv", _path);
	str_outfile = fopen(streamFileName,"w");
	if (str_outfile == NULL)
	{
		//cout << "FATAL ERROR: impossible to open output file 'str_data.txt'." << endl;
		exit(YARP_FAIL);
	}
	fprintf(str_outfile,"frame_n");
	fprintf(str_outfile,";x;y;z;azimuth;elevation;roll");
	fprintf(str_outfile,";thumb_inner;thumb_middle;thumb_outer");
	fprintf(str_outfile,";index_inner;index_middle;index_outer");
	fprintf(str_outfile,";middle_inner;middle_middle;middle_outer");
	fprintf(str_outfile,";ring_inner;ring_middle;ring_outer");
	fprintf(str_outfile,";pinky_inner;pinky_middle;pinky_outer");
	fprintf(str_outfile,";%thumh_index_abd;index_middle_abd;middle_ring_abd;ring_pinky_abd;palm_abd");
	fprintf(str_outfile,";%palm_arch");
	fprintf(str_outfile,";%wrist_pitch;wrist_yaw");
	fprintf(str_outfile,";%thumb_press;index_press;other1_press;other2_press");
	fprintf(str_outfile, "\n");

	while (!IsTerminated())
	{
		_data_inport.Read();
		_img_inport.Read();
		j++;
		str_data = _data_inport.Content();
		writeDataToFile(str_outfile,str_data,j);
		str_img.Refer(_img_inport.Content());
		sprintf(streamFileName,"%s\\image%03d.pgm",_path, j);
		YARPImageFile::Write(streamFileName, str_img,YARPImageFile::FORMAT_PPM);
	}
	
	fclose(str_outfile);

	return;
}

void CSaverThread::writeDataToFile(FILE* pFile, MNumData data, int i)
{
	fprintf(pFile,"%d",i);
	fprintf(pFile,";%f;%f;%f;%f;%f;%f", data.tracker.x, data.tracker.y, data.tracker.z, data.tracker.azimuth, data.tracker.elevation, data.tracker.roll);
	fprintf(pFile,";%d;%d;%d", data.glove.thumb[0], data.glove.thumb[1], data.glove.thumb[2]);
	fprintf(pFile,";%d;%d;%d", data.glove.index[0], data.glove.index[1], data.glove.index[2]);
	fprintf(pFile,";%d;%d;%d", data.glove.middle[0], data.glove.middle[1], data.glove.middle[2]);
	fprintf(pFile,";%d;%d;%d", data.glove.ring[0], data.glove.ring[1], data.glove.ring[2]);
	fprintf(pFile,";%d;%d;%d", data.glove.pinkie[0], data.glove.pinkie[1], data.glove.pinkie[2]);
	fprintf(pFile,";%d;%d;%d;%d;%d", data.glove.abduction[0], data.glove.abduction[1], data.glove.abduction[2], data.glove.abduction[3], data.glove.abduction[4]);
	fprintf(pFile,";%d", data.glove.palmArch);
	fprintf(pFile,";%d;%d", data.glove.wrist[0], data.glove.wrist[1]);
	fprintf(pFile,";%d;%d;%d;%d", data.pressure.channelA, data.pressure.channelB, data.pressure.channelC, data.pressure.channelD);
	fprintf(pFile, "\n");
}