#include <PA9.h>
#include "bgdata.h"


struct BMPHeader
{
    char bfType[2];       /* "BM" */
    int bfSize;           /* Size of file in bytes */
    int bfReserved;       /* set to 0 */
    int bfOffBits;        /* Byte offset to actual bitmap data (= 54) */
    int biSize;           /* Size of BITMAPINFOHEADER, in bytes (= 40) */
    int biWidth;          /* Width of image, in pixels */
    int biHeight;         /* Height of images, in pixels */
    short biPlanes;       /* Number of planes in target device (set to 1) */
    short biBitCount;     /* Bits per pixel (24 in this case) */
    int biCompression;    /* Type of compression (0 if no compression) */
    int biSizeImage;      /* Image size, in bytes (0 if no compression) */
    int biXPelsPerMeter;  /* Resolution in pixels/meter of display device */
    int biYPelsPerMeter;  /* Resolution in pixels/meter of display device */
    int biClrUsed;        /* Number of colors in the color table (if 0, use 
                             maximum allowed by biBitCount) */
    int biClrImportant;   /* Number of important colors.  If 0, all colors 
                             are important */
};
void CreateSaveScreen(char *filename,bool write)
{
	DC_FlushRange(PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(0,PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	PA_ResetSpriteSysScreen(0);
	PA_DeleteBg(0,0);
	PA_DeleteBg(0,1);
	PA_DeleteBg(0,2);
	if(write==1)
	{
		sprintf(InfoText,"Writing: %s",filename);
		PA_16bitText(0,0,0, 255,192, InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);	
	}
	else
	{
		sprintf(InfoText,"Reading: %s",filename);
		PA_16bitText(0,0,0, 255,192, InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);	
	}
}
void DeleteSaveScreen(void)
{
	LoadMainInterFace();
	if(Box.Created==1)Box.Created=0;
	//PA_ResetSpriteSysScreen(0);
	//RefreshDrawing(DSBmp.ScrollX, DSBmp.ScrollY,0);
	//DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);	
	//PA_SetBgRot(0, Layer_Draw,DSBmp.HWScrollX,DSBmp.HWScrollY,0,0,0, Draw.Zoom);
	//CheckGrid();
}

s16 CountPalCols(u16* pal)
{
	if(DSBmp.Bits==8){
		int i=0,count=0;
		for(i=0;i<256;i++){
			if(pal[i]!=0)count++;
		}
		return count+1;
	}
	return 0;
}

void SaveEPFFile(char *filename)
{
	sprintf(LastFile,"%s",filename);
	CreateSaveScreen(filename,1);

	//char templocation[256];
	//bool dir=PA_Locate("/","pics",true,10,templocation);
	//if(!dir)mkdir("/pics", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);//if there is no directory, create one
	char tempfilename[256];
	sprintf(tempfilename,"%s%s.epf",Settings.imagepath,filename);
	
	FILE* file=fopen(tempfilename, "r+");//check if file already exists
	if(file==NULL)
	{
		file=fopen(tempfilename, "wb");//if doesn't exist then create file
		DSBmp.InitialSave=0;
		if(file==NULL)
			BAG_Error_Print(Error_CreateFile);
	}
	
	PA_WaitForVBL();
	
	int i=0,y=0;
	struct EPFheader EPF;
	//fill the header with appropriate information and write it
	strcpy(EPF.Header,"EPF");//file identifier
	EPF.Bits=DSBmp.Bits;//bit mode
	EPF.Layers=DSBmp.NumOfLayers;//number of layers
	EPF.Width=DSBmp.Width;//image width
	EPF.Height=DSBmp.Height;//image height
	EPF.NColors = CountPalCols(palette);
	
	fwrite(&EPF.Header,4,1, file);//write struct to file
	fwrite(&EPF.Bits,1,1, file);//write struct to file
	fwrite(&EPF.Layers,1,1, file);//write struct to file
	fwrite(&EPF.Width,2,1, file);//write struct to file
	fwrite(&EPF.Height,2,1, file);//write struct to file
	fwrite(&EPF.NColors,1,1, file);
	
	//write the palette
	if(EPF.Bits==8)
		fwrite(&palette,1,256<<1, file);
	
	//write data
	if(Settings.FastSaving==0)
		DSBmp.InitialSave=0;
		
	if(DSBmp.InitialSave==0)
		for(i=0;i<EPF.Height;i++)
			LinesToWrite[i]=1;
	
	for(y=0;y<DSBmp.Height;y++)
	{
		if(LinesToWrite[y]==0)fseek (file,DSBmp.Width*2,SEEK_CUR);
		if(LinesToWrite[y]==1)
			for(i=0;i<DSBmp.NumOfLayers+1;i++)
				fwrite((u16*)&bglayer[LayerOrder[i]][y* DSBmp.Width],1,DSBmp.Width*2, file);

		LoadingBar(0,28,80,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),y,EPF.Height);
	}
	PA_WaitForVBL();PA_WaitForVBL();
	fclose(file);
	DSBmp.InitialSave=1;
	for(i=0;i<EPF.Height;i++)
		LinesToWrite[i]=0;
	
	LoadMainInterFace();
}
	
int screenshotbmp(char* filename, s8 Bits)
{
	CreateSaveScreen(filename,1);
	char tempfilename[256];
	sprintf(tempfilename,"%s%s.bmp",Settings.imagepath,filename);
	
	//char templocation[256];
	//bool dir=PA_Locate("/","pics",true,10,templocation);
	//if(!dir)mkdir("/pics", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);//if there is no directory, create one
	FILE* file=fopen(tempfilename, "r+");//create file
	if(file==NULL)
	{
		fclose(file);
		file=fopen(tempfilename, "w");//create file
		DSBmp.InitialSave=0;
		if(file==NULL)
			BAG_Error_Print(Error_CreateFile);
	}
	PA_WaitForVBL();
	//write headers
    int i, j;
    int width=DSBmp.Width,height=DSBmp.Height;
    int bytesPerLine;
    u8 *line;

    struct BMPHeader bmph;
    
	bytesPerLine = (((width* Bits) +31)/32)*4;
    strcpy(bmph.bfType, "BM");
	switch(Bits)
	{
		default:
			bmph.bfOffBits = 54;
		break;
		case 8:
			bmph.bfOffBits = 54+(256*4);
		break;
	}
    bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
    bmph.bfReserved = 0;
    bmph.biSize = 40;
    bmph.biWidth = width;
    bmph.biHeight = height;
    bmph.biPlanes = 1;
    bmph.biBitCount = Bits;
    bmph.biCompression = 0;
    bmph.biSizeImage = bytesPerLine * height;
    bmph.biXPelsPerMeter = 0;
    bmph.biYPelsPerMeter = 0;
    bmph.biClrUsed = 0;       
    bmph.biClrImportant = 0; 
	
	if(DSBmp.Bits<=8)
		bmph.biClrUsed = CountPalCols(palette);
	
    if (file == NULL) 
		BAG_Error_Print(Error_FileExist);
	//return(0);
  
    fwrite(&bmph.bfType, 2, 1, file);
    fwrite(&bmph.bfSize, 4, 1, file);
    fwrite(&bmph.bfReserved, 4, 1, file);
    fwrite(&bmph.bfOffBits, 4, 1, file);
    fwrite(&bmph.biSize, 4, 1, file);
    fwrite(&bmph.biWidth, 4, 1, file);
    fwrite(&bmph.biHeight, 4, 1, file);
    fwrite(&bmph.biPlanes, 2, 1, file);
    fwrite(&bmph.biBitCount, 2, 1, file);
    fwrite(&bmph.biCompression, 4, 1, file);
    fwrite(&bmph.biSizeImage, 4, 1, file);
    fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biClrUsed, 4, 1, file);
    fwrite(&bmph.biClrImportant, 4, 1, file);
	
	//write palette for 8 bit
	if(Bits==8)
	{
		temp_pal= calloc(256*4, sizeof(char));
		for (i = 0; i < 256; i++)
		{
			temp_pal[i*4]=(palette[i]>>10&31)*8;
			temp_pal[(i*4)+1]=(palette[i]>>5&31)*8;
			temp_pal[(i*4)+2]=(palette[i]&31)*8;
		}
		fwrite(temp_pal, 1, 256*4, file);
		free(temp_pal);
	}
	PA_WaitForVBL();PA_WaitForVBL();
    line = malloc(bytesPerLine);
    if (line == NULL)
	    BAG_Error_Print(Error_ImageAlloc);

	int z=0;
	u16 color=0;
	
	if(Settings.FastSaving==0)
		DSBmp.InitialSave=0;
	if(DSBmp.InitialSave==0)
	{
		for(i=0;i<height;i++)
			LinesToWrite[i]=1;
	}
	//collect the colors and write line by line
    for (i = height - 1; i >= 0; i--)
    {
		if(LinesToWrite[i]==0)
			fseek (file,bytesPerLine,SEEK_CUR);
			
		if(LinesToWrite[i]==1)
		{
			for (j = 0; j < width+1; j++)
			{
				for(z=0;z<=DSBmp.NumOfLayers;z++)
				{
					if(Bits>8)
					{
						if(bglayer[LayerOrder[z]][j + ((i) * DSBmp.Width)]!=Trans24bit)
							color= bglayer[LayerOrder[z]][(j) + ((i) * DSBmp.Width)];
					}
					else if(bglayer[LayerOrder[z]][j + ((i) * DSBmp.Width)]!=Trans8bit)
						color= bglayer[LayerOrder[z]][(j) + ((i) * DSBmp.Width)];
				}
				if(Bits>16)
				{
					s16 blue=(color&31)<<3;
					s16 green=((color>>5)&31)<<3;
					s16 red=((color>>10)&31)<<3;
					line[3*j] = red;
					line[3*j+1] = green;
					line[3*j+2] = blue;
				}
				else if(Bits==16)
				{
					//colors to convert
					s16 red=(color>>10)&31;
					s16 green=((color>>5)&31);
					s16 blue=(color&31);

					u16 NEWColor = ((((blue))<<10)|(((green))<<5)|((red)));
					u16 hibyte = (NEWColor  >> 8);   
					u16 lobyte = NEWColor  - (hibyte << 8);
					

					line[2*j]=lobyte;
					line[(2*j)+1]=hibyte;
				}
				else if(Bits==8)
					line[j]=color;
			}
			fwrite(line, 1, bytesPerLine, file);
		}
		LoadingBar(0,28,80,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),(height-i),height);
		//sprintf(InfoText,"Rows Written: %d/%d",height-i,height);
		//PA_16bitText(0,0,TextHeight, 255,192, InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);	
    }
	PA_WaitForVBL();PA_WaitForVBL();
    free(line);
	line=NULL;
	PA_WaitForVBL();PA_WaitForVBL();
    fclose(file);
	PA_WaitForVBL();PA_WaitForVBL();
	DSBmp.InitialSave=1;
	
	for(i=0;i<height;i++)
		LinesToWrite[i]=0;

	LoadMainInterFace();
    return(1);
}
