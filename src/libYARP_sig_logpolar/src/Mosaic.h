struct Images{
	unsigned char * LP_Image[2]; 
	unsigned char * Small_Remapped[2]; 
	unsigned char * Small_Mosaic; 
	unsigned char * Large_Mosaic; 
	unsigned char * Corr_Map;
};

struct Tables{
	int * RemapMap[5];
	unsigned char * Small_Mosaic_Map;
	unsigned char * Large_Mosaic_Map;
	unsigned char * Small_Partial_Map;
	unsigned char * Large_Partial_Map;
	unsigned char * LP_Map; 
	double * Raw_Corr_Map;
	int * Positions;
	int LgMosaicX;
	int LgMosaicY;
	int LgMosaicSize;
	int SmMosaicX;
	int SmMosaicY;
	int SmMosaicSize;
	int RemLUTFullX[5];
	int RemLUTFullY[5];
	int RemLUTFullXY[5];
};

void Init_Mosaic(Tables * TabPtr, Images * ImgPtr, Image_Data * Param, int ImgID);
int Corr_w_Mosaic(Tables * TabPtr, Images * ImgPtr, Image_Data * Param, int ImgID);
