
#include <yarp/LogPolarSDK.h>
#include <yarp/YARPDisparity.h>


int main(int argc, char *argv[])
{
	YARPImageOf<YarpPixelMono> LBWimg;
	YARPImageOf<YarpPixelMono> RBWimg;

	YARPImageOf<YarpPixelBGR> Limg;
	YARPImageOf<YarpPixelBGR> Rimg;

	YARPImageOf<YarpPixelBGR> Loimg;
	YARPImageOf<YarpPixelBGR> Roimg;

	YARPImageOf<YarpPixelBGR> Shifted;

	YARPImageOf<YarpPixelMono> Histo;

	Neighborhood	* WMap;
	FILE * fin;

	char filename[256];
	char path[256];
	int ImageNumber = 6666;
	ACE_OS::sprintf(path,"%s","c:/temp/tables/prefix/");

	int disparityval;

	YARPDisparityTool Test;

//Color Images
	/*
	ACE_OS::sprintf (filename, "C:/Temp/Images/R2/leftC01.ppm\0");
	YARPImageFile::Read (filename, Limg);
	ACE_OS::sprintf (filename, "C:/Temp/Images/R2/rightc01.ppm\0");
	YARPImageFile::Read (filename, Rimg);
	*/
////////////////////////////////////////////////////////////////////
//BW Images
 	ACE_OS::sprintf (filename, "C:/Temp/Images/R2/leftC%d%s", ImageNumber,".pgm\0");
	YARPImageFile::Read (filename, LBWimg);
	ACE_OS::sprintf (filename, "C:/Temp/Images/R2/rightC%d%s",ImageNumber,".pgm\0");
	YARPImageFile::Read (filename, RBWimg);


	Limg.Resize(LBWimg.GetWidth(),LBWimg.GetHeight());
	Rimg.Resize(RBWimg.GetWidth(),RBWimg.GetHeight());

	ACE_OS::sprintf(filename,"%s%s%02d_P%d%s",path ,"WeightsMap",4,8,".gio");

	if ((fin = ACE_OS::fopen(filename,"rb")) != NULL)
	{
		WMap = (Neighborhood *) malloc (LBWimg.GetWidth()*LBWimg.GetHeight() * 4 * 3 * sizeof(Neighborhood));
		ACE_OS::fread(WMap,sizeof(Neighborhood),LBWimg.GetWidth()*LBWimg.GetHeight() * 4 * 3,fin);
		ACE_OS::fclose (fin);
	}
	else
		WMap = NULL;

	Reconstruct_Color(	(unsigned char*)Limg.GetRawBuffer(),
						(unsigned char*)LBWimg.GetRawBuffer(),
						LBWimg.GetHeight(),
						LBWimg.GetWidth(),
						8,
						WMap,
						4);

	Reconstruct_Color(	(unsigned char*)Rimg.GetRawBuffer(),
						(unsigned char*)RBWimg.GetRawBuffer(),
						RBWimg.GetHeight(),
						RBWimg.GetWidth(),
						8,
						WMap,
						4);

	ACE_OS::sprintf (filename, "C:/Temp/Images/R2/Processed/leftC%d%s", ImageNumber,"Color.ppm\0");
	YARPImageFile::Write (filename, Limg);
	ACE_OS::sprintf (filename, "C:/Temp/Images/R2/Processed/rightC%d%s",ImageNumber,"Color.ppm\0");
	YARPImageFile::Write (filename, Rimg);


/////////////////////////////////////////////////////////////////////	
	Loimg.Resize(Limg.GetWidth()/4,Limg.GetHeight()/4);
	Roimg.Resize(Rimg.GetWidth()/4,Rimg.GetHeight()/4);
	Shifted.Resize(Loimg.GetWidth(),Loimg.GetHeight());
	Histo.Resize(256,64);
//	Histo.Resize(512,512);


	Test._imgL = Test.lpInfo(256,256,152,252,42,1090,1.00,8);
	Test._imgS = Test.lpInfo(256,256,152,252,42,1090,4.00,8);

	Test._actRings = 84 / (int)(Test._imgS.Ratio);

//	Build_Shift_Map(&Test._imgS,path);

	Test.loadShiftTable(&Test._imgS);
	Test.loadDSTable(&Test._imgL);

	long tempo = Get_Time();
//	for (int y=0; y<1; y++)
//	{
		Test.downSample(Limg, Loimg);
		Test.downSample(Rimg, Roimg);
	if (0)//correct left and right with '0'
		disparityval = Test.computeDisparity(Loimg,Roimg);
	else//swap
		disparityval = Test.computeDisparity(Roimg,Loimg);
//	}
	tempo = Get_Time()-tempo;
	
//	disparityval = Test._shiftFunction[disparityval]+Test._shiftLevels/2;
	printf("%f\n",tempo/100.0);
	printf("%d\n",disparityval);

	double ss = Test._gMagn;

	FILE *fout;
	fout = fopen("c:\\tempdata.txt","wa");
	fprintf(fout,"0,0,%f,",Test._gMagn);
	fprintf(fout,"%f,",Test._gMean);
	fprintf(fout,"%f,",Test._gSigma);
	fprintf(fout,"%f,",Test._snRatio);
	fprintf(fout,"%f\n",Test._squareError);

	int k;
	for (k=0; k<Test._shiftLevels; k++)
	{
		fprintf(fout,"%f,",3.0-Test._corrFunct[k]);
		fprintf(fout,"%f\n",3.0-Test._gaussFunction[k]);
	}

	fclose (fout);

/*	unsigned char * Shiftedptr = (unsigned char*)Shifted.GetRawBuffer();
	unsigned char * Origptr    = (unsigned char*)  Roimg.GetRawBuffer();
	int i,j;

	int APS = (computePadSize(3*Test._imgS.Size_Theta,Test._imgS.padding)-3*Test._imgS.Size_Theta);
 	for (j=0; j<Test._imgS.Size_Rho; j++)
	{
 		for (i=0; i<Test._imgS.Size_Theta; i++)
		{
			* Shiftedptr++   = Origptr[Test._shiftMap[(disparityval)*1*Test._imgS.Size_LP+(j*Test._imgS.Size_Theta+i)]];
			* Shiftedptr++   = Origptr[Test._shiftMap[(disparityval)*1*Test._imgS.Size_LP+(j*Test._imgS.Size_Theta+i)]+1];
			* Shiftedptr++   = Origptr[Test._shiftMap[(disparityval)*1*Test._imgS.Size_LP+(j*Test._imgS.Size_Theta+i)]+2];
		}
		Shiftedptr+=APS;
	}
	
	
	ACE_OS::sprintf (filename, "C:\\Temp\\Images\\testpad\\ShiftTest.ppm\0");
	YARPImageFile::Write (filename, Shifted);
*/
	Test.makeHistogram(Histo);

	ACE_OS::sprintf (filename, "C:/Temp/Images/R2/Processed/Histogram%d%s", ImageNumber,".pgm\0");
	YARPImageFile::Write (filename, Histo);
	return 0;
}

