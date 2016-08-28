#include <PA9.h>
#include "version.h"
#include "bgdata.h"

int DebugTimer=0;
void UserDumpDebug(void)
{
	//debug
	if(Pad.Newpress.R)
		DebugTimer=0;
	if(Pad.Held.R)
	{
		DebugTimer++;
		if(DebugTimer>60)BAG_Error_Print(Error_UserSpecified);
	}
	UpdateTB();
	s8 i=0;
	for(i=0;i<2;i++)
		FadeInterface(Fade.Style[i],Fade.ColWheel[i]);
}

char *_getDeviceType(char *partition, char *device) 
{ 
  struct stat st; 
  if(stat(partition, &st) == 0) 
  { 
    strncpy(device, (char *)&st.st_dev, 4); 
    device[4] = '\0'; 
  }
  else 
  { 
    // Something went wrong 
    strcpy(device, "N/A"); 
  } 
  return device; 
} 


void BAG_Error_Print(s8 ErrorType)
{
	//clear the screen
	PA_VBLFunctionReset();
	dmaFillHalfWords(0,PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(0,PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	PA_ResetSpriteSys();
	u8 i=0;
	for(i=0;i<128;i++)
		PA_SetSpriteXY(0,i,-64,-64);
	PA_DeleteBg(0,0);
	PA_DeleteBg(0,1);
	PA_DeleteBg(0,2);
	PA_DeleteBg(1,2);

	PA_SetBgRot(0, Layer_Draw,0,0,0,0,0,256);
	PA_WaitForVBL();
	
	char Message[256];
	memset(Message,0,255);
	sprintf(Message,"Uh oh, Etch has encountered an Error!");
	PA_16bitText(0,0,0, 255,192, Message, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);
	
	switch(ErrorType)
	{
		case Error_ImageAlloc:
			sprintf(Message,"'Failed to allocate memory for image.'");
		break;
		case Error_FontAlloc:
			sprintf(Message,"'Failed to allocate memory for font.'");	
		break;
		case Error_UndoAlloc:
			sprintf(Message,"'Failed to allocate memory for undos.'");			
		break;		
		case Error_BmpLoad:
			sprintf(Message,"'Failed to load image type 'BMP''.");		
		break;
		case Error_EpfLoad:
			sprintf(Message,"'Failed to load image type 'EPF''.");			
		break;
		case Error_RamInit:
			sprintf(Message,"'Ram not recognized'");			
		break;
		case Error_OldIni:
			sprintf(Message,"'INI outdated, ensure latest version is used'");	
		break;
		case Error_Fat:
			sprintf(Message,"'Failed to initiate FAT, check DLDI settings'");	
		break;	
		case Error_CreateFile:
			sprintf(Message,"'Failed to create specified file'");	
		break;
		case Error_FileExist:
			sprintf(Message,"'Specified file failed to open'");	
		break;
		case Error_BucketAlloc:
			sprintf(Message,"'Failed to allocate memory for bucket'");	
		break;
		case Error_ShapeAlloc:
			sprintf(Message,"'Failed to allocate memory for shapes'");	
		break;
		case Error_BrowserAlloc:
			sprintf(Message,"'Failed to allocate memory for file browser'");	
		break;
		case Error_OggListAlloc:
			sprintf(Message,"'Failed to allocate memory for ogg list'");	
		break;
		case Error_UserSpecified:
			sprintf(Message,"'User has identified an error occuring'");
		break;
		case Error_RamError:
			sprintf(Message,"'Ram Error, overclock not supported'");
		break;
		case Error_FadeError:
			sprintf(Message,"'An error with fading has occured'");
		break;
		case Error_FontLoadAlloc:
			sprintf(Message,"'Failed to allocate memory for font'");
		break;
		case Error_NotSupportedImage:
			sprintf(Message,"Not a supported image format'");
		break;
	}
	PA_16bitText(0,0,16, 255,192, Message, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);
	
	sprintf(Message,"Please wait, dumping Data to 'EtchLog.txt' on root");
	PA_16bitText(0,0,32, 255,192, Message, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);	

	char card[5];
	_getDeviceType("fat:/",card);
	
	FILE* file=fopen("fat:/EtchLog.txt", "a");//create file

	//print date and time
	fprintf (file,"[%02d/%02d - %02d:%02d.%02d]\n",PA_RTC.Month,PA_RTC.Day,PA_RTC.Hour,PA_RTC.Minutes, PA_RTC.Seconds);
	//start with program version
	int BuildNum= VERSION_EXTEND;
	fprintf (file,"Etch Build %d\n",BuildNum);
	//print error number
	fprintf (file,"Error #%d\n",ErrorType);
	//print card make
	fprintf (file,"Device ID %s\n",card);
	//print some general program info
	fprintf (file,"Image Size: %dx%d\n",DSBmp.Width,DSBmp.Height);
	fprintf (file,"Layers: %d, Undos: %d\n",DSBmp.NumOfLayers, DSBmp.NumOfUndos);
	fprintf (file,"Image Bit Depth: %d\n",DSBmp.Bits);
	if(Settings.ExternalRam)
	{
		fprintf (file,"Ram Detected: %s\n",ram_type_string ());
		fprintf (file,"Ram Speed: %d\n",Settings.RamSpeed);
	}
	fprintf (file,"Current Tool: %d,%d\n",Box.ToolSelected,Box.ToolMode);
	//which menu the program crashed in
	fprintf (file,"Location: %d\n",DSBmp.PrgmLocation);
	

	//print blank line at end of log
	fprintf (file," \n");
	fclose(file);
	sprintf(Message,"It is now safe to shut the DS off.");
	PA_16bitText(0,0,40, 255,192, Message, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);		
	bool loop=1;
	while(loop)
	{
		PA_WaitForVBL();
	}
	//exit(0);
}