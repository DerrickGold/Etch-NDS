#include <PA9.h>
#include "version.h"
#include "bgdata.h"

#define CurrentIniVersion 2


s8 LoadProgramSettings(char *inifile){
	char NewFileName[256];
	sprintf(NewFileName,"%s%s",Settings.iniPath,inifile);
	//check existance of Ini file
	FILE* testread= fopen (NewFileName, "rb"); //rb = read
	//if an external file does not exist, use internal file
	if(!testread){
		testread=NULL;
		sprintf(NewFileName,"nitro:/default.ini");
		testread= fopen (NewFileName, "rb");
		if(!testread)return 0;
		chdir("nitro:/");
	}
	else {
		fclose(testread);
		chdir("fat:/");
	}
	//Version
	Settings.IniVersion = ini_getl("Version", "Ini_Version",0,NewFileName);
	if(Settings.IniVersion!=CurrentIniVersion)return -1;
	
	//misc
	//auto grid
	Settings.AutoGrid = ini_getl("Misc", "Zoom_Auto_Grid",0,NewFileName);
	Settings.DefaultX = ini_getl("Misc", "Default_X",0,NewFileName);
	Settings.DefaultY = ini_getl("Misc", "Default_Y",0,NewFileName);
	Settings.FastScroll = ini_getl("Misc", "Fast_Scroll",0,NewFileName);
	Settings.FastSaving = ini_getl("Misc", "Fast_Save",0,NewFileName);
	Settings.UpdateTimer = ini_getl("Misc", "Time_Mini_View",0,NewFileName);
	Settings.EnableMenuFade = ini_getl("Misc", "Fade_Menus",0,NewFileName);
	//font stuff
	Settings.FontMaxSize= ini_getl("Fonts", "Font_MaxSize",0,NewFileName);
	Settings.FontMinSize= ini_getl("Fonts", "Font_MinSize",0,NewFileName);
	
	Settings.ExternalRam =ini_getl("Memory Management", "ExtMem",0,NewFileName);
	//detect ram
	if(Settings.ExternalRam && ram_init (DETECT_RAM)){
		Settings.ExternalRam=1;	
		//set image memory to ram limits
		Settings.MaxBmpSize=(ram_size()>>1);
		Settings.MaxMemory=ram_size();
		//overclock ram
		Settings.RamSpeed = ini_getl("Memory Management", "ExtMemOc",0,NewFileName);
		//check whether to check ram
		Settings.TestRam= ini_getl("Memory Management", "ExtMemTest",1,NewFileName);
		//set allocated font memory to allocated image memory
		Settings.FontFileSize= ini_getl("Memory Management", "Allocated_Memory",0,NewFileName);
		//change flood depth
		Settings.Flood_Depth=8000;
		//configure ram
		DSBmp.ExtRam = ram_unlock ();
		setSlot2Speed(speedMenuValues[Settings.RamSpeed]);
		#ifdef USE_BGET
			bpool(DSBmp.ExtRam,ram_size()); //set memory allocator for ram
		#endif
		ram_lock();
		DSBmp.ExtRam=NULL;
	}
	else{
		Settings.ExternalRam=0;
		//default memories for images, fonts and flood depth
		Settings.MaxBmpSize = ini_getl("Memory Management", "Max_Pixel_Count",0,NewFileName);
		Settings.MaxMemory = ini_getl("Memory Management", "Allocated_Memory",0,NewFileName);
		Settings.FontFileSize= ini_getl("Fonts", "Font_MaxFileSize",0,NewFileName);
		Settings.Flood_Depth=2000;		
	}

	Settings.MaxBuffers = ini_getl("Memory Management", "MaxBuffers",0,NewFileName);
	Settings.SDCache = ini_getl("Memory Management", "SD_Undo_Cache",0,NewFileName);
	Settings.MaxLayers = ini_getl("Memory Management", "Usable_Layers",0,NewFileName);

	//appearance
	Settings.ShowMiniView = ini_getl("Main Appearance", "Show_Mini_View",0,NewFileName);

	Settings.MiniviewX = ini_getl("Main Appearance", "MiniViewX",0,NewFileName);
	Settings.MiniviewY = ini_getl("Main Appearance", "MiniViewY",0,NewFileName);
	Settings.MiniviewWidth = ini_getl("Main Appearance", "MiniViewWidth",0,NewFileName);
	Settings.MiniviewHeight = ini_getl("Main Appearance", "MiniViewHeight",0,NewFileName);	
	Settings.keepAspect = ini_getl("Main Appearance", "Keep_Aspect_Ratio",0,NewFileName);

	//back drop color
	Settings.BDRed = ini_getl("Main Appearance", "BackDrop_Red",0,NewFileName);
	Settings.BDGreen = ini_getl("Main Appearance", "BackDrop_Green",0,NewFileName);
	Settings.BDBlue = ini_getl("Main Appearance", "BackDrop_Blue",0,NewFileName);
	PA_SetBgPalCol(1, 0,PA_RGB(Settings.BDRed,Settings.BDGreen,Settings.BDBlue));
	PA_SetBgPalCol(0, 0,PA_RGB(Settings.BDRed,Settings.BDGreen,Settings.BDBlue));
	//text color
	Settings.TxtRed = ini_getl("Main Appearance", "Text_Red",0,NewFileName);
	Settings.TxtGreen = ini_getl("Main Appearance", "Text_Green",0,NewFileName);
	Settings.TxtBlue = ini_getl("Main Appearance", "Text_Blue",0,NewFileName);	
	Settings.LrgTxtR = ini_getl("Main Appearance", "Large_Txt_R",0,NewFileName);
	Settings.LrgTxtG = ini_getl("Main Appearance", "Large_Txt_G",0,NewFileName);
	Settings.LrgTxtB = ini_getl("Main Appearance", "Large_Txt_B",0,NewFileName);
	//highlight
	Settings.HighlightRed = ini_getl("Main Appearance", "Highlight_Red",0,NewFileName);
	Settings.HighlightGreen = ini_getl("Main Appearance", "Highlight_Green",0,NewFileName);
	Settings.HighlightBlue = ini_getl("Main Appearance", "Highlight_Blue",0,NewFileName);
	
	//PA_SetTextCol(1,Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue);
	//keyboard color
	//letters
	Settings.KeyboardCol[0] = PA_RGB(ini_getl("Keyboard Colors", "kb_Letter_R",0,NewFileName),
									 ini_getl("Keyboard Colors", "kb_Letter_G",0,NewFileName),
									 ini_getl("Keyboard Colors", "kb_Letter_B",0,NewFileName));
	//pressed letters
	Settings.KeyboardCol[1] = PA_RGB(ini_getl("Keyboard Colors", "kb_Pressed_Letter_R",0,NewFileName),
									 ini_getl("Keyboard Colors", "kb_Pressed_Letter_G",0,NewFileName),
									 ini_getl("Keyboard Colors", "kb_Pressed_Letter_B",0,NewFileName));
	//keys
	Settings.KeyboardCol[2] = PA_RGB(ini_getl("Keyboard Colors", "kb_Key_R",0,NewFileName),
									 ini_getl("Keyboard Colors", "kb_Key_G",0,NewFileName),
									 ini_getl("Keyboard Colors", "kb_Key_B",0,NewFileName));
	//pressed keys
	Settings.KeyboardCol[3] = PA_RGB(ini_getl("Keyboard Colors", "kb_Pressed_Key_R",0,NewFileName),
									 ini_getl("Keyboard Colors", "kb_Pressed_Key_G",0,NewFileName),
									 ini_getl("Keyboard Colors", "kb_Pressed_Key_B",0,NewFileName));
	//file browsing
	Settings.ScanIMGData = ini_getl("File Management", "Get_Img_Data",0,NewFileName);
	
	ini_gets("File Management","Audio_Folder","blank",(char*)Settings.audiopath,256,NewFileName);
	if(strcmp(Settings.audiopath,"blank"))Settings.EnableAudio=1;
	else Settings.EnableAudio=0;
	
	char TempPath[256];
	ini_gets("File Management","Picture_Folder","fat:/",(char*)TempPath,256,NewFileName);
	sprintf(Settings.imagepath,"fat:/%s",TempPath);
	ini_gets("File Management","Font_Folder","fat:/",(char*)TempPath,256,NewFileName);
	sprintf(Settings.fontpath,"fat:/%s",TempPath);
	
	Settings.MaxFiles = ini_getl("File Management", "Dir_Max_Files",0,NewFileName);
	Settings.PreviewWd = ini_getl("File Management", "Preview_Width",0,NewFileName);
	Settings.PreviewHt = ini_getl("File Management", "Preview_Height",0,NewFileName);
	

	
	
	//debug
	Settings.ZoomLevel=256;
	Draw.Alpha=256;
	
	//controls
	//shoulders
	ini_gets("Controls","R_Button",NULL,(char*)Custom.R_Button,MaxControlChars,NewFileName);
	ini_gets("Controls","L_Button",NULL,(char*)Custom.L_Button,MaxControlChars,NewFileName);
	//face
	ini_gets("Controls","A_Button",NULL,(char*)Custom.A_Button,MaxControlChars,NewFileName);
	ini_gets("Controls","B_Button",NULL,(char*)Custom.B_Button,MaxControlChars,NewFileName);
	ini_gets("Controls","X_Button",NULL,(char*)Custom.X_Button,MaxControlChars,NewFileName);
	ini_gets("Controls","Y_Button",NULL,(char*)Custom.Y_Button,MaxControlChars,NewFileName);
	//dpad
	ini_gets("Controls","Right_Dpad",NULL,(char*)Custom.Right_Dpad,MaxControlChars,NewFileName);
	ini_gets("Controls","Left_Dpad",NULL,(char*)Custom.Left_Dpad,MaxControlChars,NewFileName);
	ini_gets("Controls","Up_Dpad",NULL,(char*)Custom.Up_Dpad,MaxControlChars,NewFileName);
	ini_gets("Controls","Down_Dpad",NULL,(char*)Custom.Down_Dpad,MaxControlChars,NewFileName);

	Draw.Color[0]=PA_RGB(0,0,0);
	Draw.Color[1]=PA_RGB(31,31,31);
	DSBmp.Bits=24;
	return 1;
}
void ShowMemory(void){
	PA_SetTextCol(1,31,31,31);
	struct mallinfo info = mallinfo(); 
	PA_OutputText(1,0,1,"%d\n%d ",info.usmblks + info.uordblks,info.usmblks + info.uordblks);
}

char* FatLoad(char *filename)
{
	FILE* File = fopen (filename, "rb"); 
	if(File)// Make sure there is a file to load
	{ 
		u32 Size;
		fseek (File, 0 , SEEK_END);
		Size = ftell (File);
		rewind (File);
		char *buffer = (char*) malloc (sizeof(char)*Size);
		if(buffer!=NULL){
			fread (buffer, 1, Size, File);

			fclose (File);
			return buffer;
		}
		else {//if this happens you're out of memory!
			fclose (File);
			return NULL;
		}
	}
	return NULL;
}

void SplashScreen(void){
	char* BMP = FatLoad("nitro:/neoflash.bmp");
	PA_LoadBmpEx (1, 0, 0, BMP);
	PA_LoadBmpEx (0, 0, 0, BMP);
	free(BMP);
}
	
//#include "daelnue.h"
void TitleScreen(void){
	PA_Init16bitBg(0,3);
	PA_Init16bitBg(1,3);
	
	u16 *ToolBuf=NULL;
	ToolBuf= (u16*) calloc (SCREEN_WIDTH*SCREEN_HEIGHT,sizeof(u16));
	
	DC_FlushRange(PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(PA_RGB(31,31,31),PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DC_FlushRange(PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(PA_RGB(31,31,31),PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DC_FlushRange(ToolBuf,(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(PA_RGB(31,31,31),ToolBuf,(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	
	if(!nitroFSInit()){
		BAG_Error_Print(Error_Fat);
	}
	/*SplashScreen();
	PA_WaitFor(Pad.Newpress.Anykey || Stylus.Newpress);*/
	DC_FlushRange(PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(PA_RGB(31,31,31),PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DC_FlushRange(PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(PA_RGB(31,31,31),PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);	
	//Font_Load((u8*)daelnue,daelnue_size,0);
	Font_FatLoad("nitro:/_fonts/daelnue.ttf",0);

	Font_SetScreenBuf(1,PA_DrawBg[1]);
	Font_OutputText(1,50,10,256,192,"BassAceGold ",1,FT_FIXED(25),0,PA_RGB(0,15,0));	
	Font_OutputText(1,136,10,256,192,"Presents:",1,FT_FIXED(25),0,PA_RGB(14,14,14));
	
//===============================================================================================================
//TopScreen==================================================================================================
//===============================================================================================================
	char temp[256];
	//main title======================================
	s16 titlex=80,titley=50, titlex2=180, titley2=100, pixelcount=0,timer=0;
	Font_SetScreenBuf(1,ToolBuf);
	Font_OutputText(1,titlex,titley,titlex2,titley2,"Etch",1,FT_FIXED(80),0,PA_RGB(31,0,0));
	//reveal title
	int x=0, y=0;
	while(pixelcount< 659){
		timer++;
		if(timer>60<<7){
			x= (rand()% 100) + 80;
			y= (rand()% 50) + 50;
			if(PA_Get16bitPixel(1, x, y) !=ToolBuf[x+ (y*SCREEN_WIDTH)] && ToolBuf[x+ (y*SCREEN_WIDTH)] != 0 ){
				PA_Put16bitPixel  (1,x,y,ToolBuf[x+ (y*SCREEN_WIDTH)]);
				pixelcount++;
				timer=0;
			}
		}
	}
	free(ToolBuf);
	ToolBuf=NULL;
	//subtitle======================================
	Font_SetScreenBuf(1,PA_DrawBg[1]);
	Font_OutputText(1,30,90,256,192,"A Pixel Art Utility",1,FT_FIXED(40),0,PA_RGB(0,0,0));
	
	//build
	
	memset(temp,0,256);
	int BuildNum= VERSION_EXTEND;
	sprintf(temp,"Build:%d ",BuildNum);
	Font_OutputText(1,100,120,256,192,temp,1,FT_FIXED(20),0,PA_RGB(14,14,14));
	
	//copy rights
	Font_OutputText(1,0,192-30,256,192,"Portions of this software are copyright 2010\nThe FreeType Project (www.freetype.org).\nAll rights reserved.",1,FT_FIXED(20),0,PA_RGB(14,14,14));	
//===============================================================================================================
//Bottom Screen==================================================================================================
//===============================================================================================================
	Font_SetScreenBuf(0,PA_DrawBg[0]);
	Font_OutputText(0,0,0,256,192,"Loading Setttings Please Wait",1,FT_FIXED(20),0,PA_RGB(14,14,14));
	LoadStylusCursor(0,3,1,TB_Bucket,(256>>1)-(16>>1),(192>>1)-(16>>1));
	

	switch(LoadProgramSettings("etch.ini")){
		default:break;//nothing
		case 0:
			Font_OutputText(0,0,20,256,192,"Failed to find Ini File",1,FT_FIXED(20),0,PA_RGB(14,14,14));
			while(1){
			}
		break;
		case -1:
			BAG_Error_Print(Error_OldIni);
		break;
	}	
	if(Settings.ExternalRam && Settings.TestRam){
		Font_OutputText(0,0,20,256,192,"Testing External Ram",1,FT_FIXED(20),0,PA_RGB(14,14,14));
		DSBmp.ExtRam = ram_unlock ();
		#ifdef USE_BGET
			u16 *Test =(u16*) bget(1024*2);	
			//test to make sure ram is working 
			if(Test!=NULL){
				brel(Test);
			}	
			else BAG_Error_Print(Error_RamError);
			Test=NULL;
		#else
			u16 Test[1024*2];
			memset (Test,1,(1024*2)<<1);
			memset ((u16*)DSBmp.ExtRam,1,(1024*2)<<1);	
			if(memcmp (Test,(u16*)DSBmp.ExtRam,(1024*2)<<1)!=0)BAG_Error_Print(Error_RamError);

		#endif	
		ram_lock();
		DSBmp.ExtRam=NULL;
	}
	DeleteCursor();
	DC_FlushRange(PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(PA_RGB(31,31,31),PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	Font_OutputText(0,30,70,256,192,"Touch to Continue",1,FT_FIXED(40),0,PA_RGB(14,14,14));
	if(Settings.ExternalRam){
		char RamType[32];
		sprintf(RamType,"%s ram detected",ram_type_string ());
		Font_OutputText(0,30,0,256,192,RamType,1,FT_FIXED(20),0,PA_RGB(14,14,14));
	}
	PA_WaitFor(Pad.Newpress.Anykey || Stylus.Newpress);
	DC_FlushRange(PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(PA_RGB(24,26,30),PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DC_FlushRange(PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(PA_RGB(24,26,30),PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	Font_FatUnload(0);
	#ifdef ENABLEOGG
		if(Settings.EnableAudio)PA_VBLFunctionInit(UpdateOgg);
		else {
			OggStatus=0;
			PA_VBLFunctionInit(UserDumpDebug);
		}
	#else 
		PA_VBLFunctionInit(UserDumpDebug);
	#endif
	PA_WaitForVBL();PA_WaitForVBL();	
}
bool GetRomPath(char* pathbuf, const char * path){
	bool test=0;
	s16 length=strlen(path);
	if(length>0){
		test=1;
		while(path[length]!='/')length--;
		strncpy(pathbuf,path, length+1);
	}
	return test;
}

int main(int argc, char ** argv){
	PA_Init();
	PA_InitVBL();
	GetRomPath(Settings.iniPath,argv[0]);

	
	PA_AddBitmapFont(5, &lucidasans);	
	PA_AddBitmapFont(6, &modernlarge);	
	PA_Add16cFont(7, &lucidasans1bit);	
	PA_Add16cFont(8, &modernlarge1bit);	
	PA_TextLineSpacing(-1);
	int i=0;
	for(i=0;i<10;i++){
		bglayer[i]=NULL;
		alphalayer[i]=NULL;
		UndoBuf[i]=NULL;
	}
	Cache.UndoBuf=NULL;
	LinesToWrite=NULL;
	#ifdef ENABLEOGG
		InitOGG();
	#endif		
	TitleScreen();

	PA_Init16cBg(1, 2);//for view box
	if(argc < 2)
		CanvasMenu();
	else
	{
		LoadMainInterFace();
		char ext[10];
		memset(ext, 0, sizeof(ext));
		int len = strlen(argv[1]) - 1;
		int i = len;
		while(len > 0 && argv[1][len] != '.')
			len--;

		strncpy(ext, &argv[1][len + 1], i - len);

		if(!strcasecmp(ext, "bmp"))
			LoadBmp(argv[1]);
		else if(!strcasecmp(ext, "epf"))
			LoadEPF(argv[1]);
		else
			BAG_Error_Print(Error_NotSupportedImage);
		
		LoadMainInterFace();
	}
		

	InitTB(0,PA_RGB(24,26,28),230-32, 10,80);
	Draw.Zoom=Settings.ZoomLevel;
	Box.Tool[TB_Zoom].SliderValue = Draw.Zoom;
	PA_SetBgRot(0, 3,0,0,0,0,0, Draw.Zoom);
	
	while(1){
		//PA_OutputText(1, 0, 0, "fps\n%d ",PA_RTC.FPS);
		UpdateStatsBar();
		Controls();
		
		//PA_vblFunc();
		PA_WaitForVBL();
	}
	return 0;

}