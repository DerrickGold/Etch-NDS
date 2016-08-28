#include <PA9.h>
#include "bgdata.h"

//File Browser Settings
s8 FBScale = 12;
s8 FB_FilesPerScreen = 14;
s8 FB_ScrollOffset = -2;
s8 FBFont = 5;

void SetFBStyle(s8 Style)
{
	switch(Style)
	{
		default:
			FBScale = 8; FB_FilesPerScreen = 22; FB_ScrollOffset = 0; FBFont = 5;
		break;
		case 1:
			FBScale = 12; FB_FilesPerScreen = 14; FB_ScrollOffset = -2; FBFont = 5;
		break;
		case 2:
			FBScale = 16; FB_FilesPerScreen = 11; FB_ScrollOffset = 0; FBFont = 6;						
		break;
	}
}



/*================================================================================================================
File Compression Functions
//===============================================================================================================*/
int dsCompress(char *inputFilename, char *outputFilename)
{
	ClearScreen(0);
	PA_16bitText(0,108,70, 255,192,"Compressing File", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);
	gzFile outFile = gzopen(outputFilename, "wb");
	FILE *inFile = fopen(inputFilename, "rb");

	//grab file size
	int FileSize=0;
	fseek (inFile , 0 , SEEK_END);
	FileSize= ftell (inFile);
	rewind (inFile); 

	char buffer[512];
	int readSize = 0;
	int err;

	int Progress=0;
	while((readSize = fread(buffer, 1, sizeof(buffer), inFile)) > 0)
	{
		LoadingBar(0,28,80,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),Progress,FileSize);
		if(gzwrite(outFile, buffer, (unsigned)readSize) != readSize)
		{
			gzerror(outFile, &err);
			break;
		}
		Progress+=readSize;
	}

	fclose(inFile);
	if(gzclose(outFile) == Z_OK)
		return Z_OK;
		
	return err;
}


int dsUncompress(char *inputFilename, char *outputFilename)
{
	ClearScreen(0);
	PA_16bitText(0,108,70, 255,192,"Decompressing File", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);
	 //grab file size which is stored in the last 4 bytes of a gz file
	 int FileSize=0;
	FILE *testFile = fopen(inputFilename, "rb");
	fseek (testFile , 0 , SEEK_END);
	fseek (testFile , -4 , SEEK_CUR);
	fread (&FileSize,4, 1,testFile);
	rewind (testFile); 
	fclose(testFile);
   
	gzFile inFile = gzopen(inputFilename, "rb");
	FILE *outFile = fopen(outputFilename, "wb");
	char *buffer[512];
	int writeSize = 0;
	int test = 0;
	int result = 0;
	int Progress=0;
   

   
	while((writeSize = gzread(inFile, buffer, sizeof(buffer))) > 0)
	{
		LoadingBar(0,28,80,200,16,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),Progress,FileSize);
		Progress+=writeSize;
		if(writeSize < 0)
		{
			gzerror(inFile, &result);
			break;
		}
		else if(writeSize == 0)
		{
			result = EOF;
			break;
		}	
		if((test = fwrite(buffer, 1, (unsigned)writeSize, outFile)) != writeSize)
		{
			result = Z_ERRNO;
			break;
		}
	}

	gzclose(inFile);
	fclose(outFile);

	if(result == EOF)
		return Z_OK;
	else if(result == Z_ERRNO)
		return Z_ERRNO;
	else
    return result;

}

/*================================================================================================================
File Browser Gui
//===============================================================================================================*/
void LoadingBar(bool screen, s16 x,s16 y,s16 width,s16 height,u16 color1,u16 color2,int count, int total)
{
	char Percentage[6];

	//draw box outline
	PA_Draw16bitLine(screen,x,y,x+width,y,color1);//top
	PA_Draw16bitLine(screen,x,y+height,x+width,y+height,color1);//bottom
	PA_Draw16bitLine(screen,x+width,y,x+width,y+height,color1);//right
	PA_Draw16bitLine(screen,x,y,x,y+height,color1);//left
	
	//percentage numbers
	int percent=(count*100)/total;
	if(percent<0 || percent>100)percent=100;
	sprintf(Percentage," %d%% ",percent);
	s16 TextPos= (width/2)-TextHeight +x;
	s16 TextYpos = (height/2) - (TextHeight/2) + y;
	//fill the inside
	u64 BarPos = (percent*width)/100;
//	((count*width)/(total));
	
	Fill16bitBlock(x+1,y+1,height-1,BarPos,color2,PA_DrawBg[0]);
	Fill16bitBlock(TextPos,TextYpos,8,22,color2,PA_DrawBg[0]);
	//clear some of the bar for text
	PA_16bitText(screen,TextPos,TextYpos,width,height,Percentage, color1, 5, 1, 100);
}
	
	

void ScrollFBText(s32 Scroll,s8 mode)
{
	int i=0;
	if(FBFont>5)
		PA_TextLetterSpacing(-5);
	
	s16 TextX = 9;
	if(FBScale>=16)
		TextX = 17;
	
	for(i=i;i<FB_FilesPerScreen;i++)
	{	
		if(i+Scroll <DSBmp.NumOfFiles)
		{
			//file names
			memset(InfoText,0,255);
			strncpy (InfoText,files[i+Scroll].filename,45);
			InfoText[45]='\0';

			Fill16bitBlock(0,LargeTextHeight+(FBScale*i),FBScale,233,0,PA_DrawBg[0]);
			Fill16bitBlock(0,LargeTextHeight+(FBScale*(i+1)),FBScale,233,0,PA_DrawBg[0]);
			//if(i%2==0 && i!=0) Fill16bitBlock(0,LargeTextHeight+(TextHeight*i),LargeTextHeight,233,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),PA_DrawBg[0]);
			if(GET_FLAG(files[i+Scroll].Flags, Is_Folder))
			{
				if(!strcmp("..",files[i+Scroll].filename))
				{
					PA_16bitText(0,TextX,LargeTextHeight+(FBScale*i), 234,192,"Back",PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), FBFont, 1, 100);
					PA_SetSpriteAnimEx (0,i+3,8,8,1,0);
				}
				else
				{
					PA_16bitText(0,TextX,LargeTextHeight+(FBScale*i), 234,192, InfoText, PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), FBFont, 1, 100);
					PA_SetSpriteAnimEx (0,i+3,8,8,1,1); 
				}
			}
			else
			{
				PA_16bitText(0,TextX,LargeTextHeight+(FBScale*i), 234,192, InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), FBFont, 1, 100);
				if(!strcmp("bmp",files[i+Scroll].ext) ||!strcmp("epf",files[i+Scroll].ext))
					PA_SetSpriteAnimEx (0,i+3,8,8,1,2); 
				else if(!strcmp("ttf",files[i+Scroll].ext))
					PA_SetSpriteAnimEx (0,i+3,8,8,1,4); 
				else if(!strcmp("gz",files[i+Scroll].ext))
					PA_SetSpriteAnimEx (0,i+3,8,8,1,5); 
			}
		}
		else 
			PA_SetSpriteAnimEx (0,i+3,8,8,1,3);
	}
	PA_TextLetterSpacing(0);
}
void CreateFBSelectBar(u16 color)
{	
	PA_SetBgPrio(0,3,1);
	//PA_LoadBackground(0, 2, &bar);
	PA_Init16cBg  (0, 2);
	PA_SetBgPrio(0,2,3);
	//PA_SetBgPalNCol (0,2,0,1, color);
	//PA_BGScrollXY(0,2,0,-16);
	//x color
	PA_SetBgPalCol(0,(0<<4)+3,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue));

}
void DrawFileWindow(s8 screen)
{
	PA_16cClearZone(screen,0,0,256,192);
	PA_SetBgPrio(screen,2,0);//set priority to high

	//draw the box
	//first the backdrop
	//backdrop color
	Fill16bitBlock(64,64,81,122, 0,PA_DrawBg[0]);
	//16 pals
	//boarder & small text color
	PA_SetBgPalCol(screen,(0<<4)+1, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue));
	//big text color
	PA_SetBgPalCol(screen,(0<<4)+2,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB)); 
	
	//horizontal lines
	PA_Draw16cLine(screen,64,64,186,64,1);
	PA_Draw16cLine(screen,64,77,186,77,1);
	PA_Draw16cLine(screen,64,145,186,145,1);
	//vertical lines
	PA_Draw16cLine(screen,64,64,64,145,1);
	PA_Draw16cLine(screen,186,64,186,145,1);
	PA_Draw16cLine(screen,165,64,165,77,1);

	//draw X
	PA_Draw16cLine(screen,171,65,183,76,3);
	PA_Draw16cLine(screen,181,65,170,76,3);
}
void DrawFileInfoBox(s8 screen, s16 filenum, u8 mode)
{

	DrawFileWindow(screen);
	//bottom buttons
		//vert
	PA_Draw16cLine(screen,64,132,186,132,1);
		//hor
	PA_Draw16cLine(screen,101,132,101,145,1);
	PA_Draw16cLine(screen,151,132,151,145,1);
	
	//draw the box header
	PA_TextLetterSpacing(-5);
	PA_16cText(screen,69,66,255, 192,"File Info:", 2,8,10);
	PA_TextLetterSpacing(0);
	//display file info
	//display the name
	PA_16cText(screen,67,81,186, 90,files[filenum].filename, 1,7,100);
	char info[256];
	memset(&info,0,255);
	
	ScanFileSizes(filenum,mode);
	//if file is not a folder
	if(!GET_FLAG(files[filenum].Flags, Is_Folder))
	{
		//Button Text, universal for all file types
		PA_16cText(screen,66,134,255, 192,"Delete", 3,7,100);
	
		if(!strcmp (files[filenum].ext,"gz"))
		{//if file is gzip
			PA_16cText(screen,154,134,255, 192,"Unzip", 3,7,100);
			PA_SetSpriteAnimEx (0,2,32,32,1,4);
			sprintf(info, "Unzipped: %d kb",FileInfo.UnZipped>>10);
			PA_16cText(screen,107,103,255, 192,info, 1,7,100);
			sprintf(info, "Zipped: %d kb",FileInfo.Size>>10);
			PA_16cText(screen,107,111,255, 192,info, 1,7,100);
		}
		//if file is not a gzip
		else
		{
			PA_16cText(screen,154,134,255, 192,"Open", 3,7,100);
			
			if(mode==FB_Image)
			{
				sprintf(info, "Bits: %d",FileInfo.Bits);
				PA_16cText(screen,107,103,255, 192,info, 1,7,100);
				sprintf(info, "Size: %d x %d",FileInfo.Width,FileInfo.Height);
			}
			else sprintf(info, "Size: %d kb",FileInfo.Size>>10);
			PA_16cText(screen,107,111,255, 192,info, 1,7,100);
			//set the icon
			if(!strcmp (files[filenum].ext,"epf"))
				PA_SetSpriteAnimEx (0,2,32,32,1,0);
			else if(!strcmp (files[filenum].ext,"bmp"))
				PA_SetSpriteAnimEx (0,2,32,32,1,1);
			else if(!strcmp (files[filenum].ext,"ttf"))
				PA_SetSpriteAnimEx (0,2,32,32,1,2);
			//if the extention is not gz
			if(strcmp (files[filenum].ext,"gz"))
				PA_16cText(screen,116,134,255, 192,"Zip", 3,7,100);
		}
	}
	//if file is actually a folder
	else
	{
		PA_16cText(screen,116,134,255, 192,"Open", 3,7,100);
		PA_SetSpriteAnimEx (0,2,32,32,1,3);
		sprintf(info, "Files: %d",DSBmp.PreviewFolderCount);
		PA_16cText(screen,107,103,255, 192,info, 1,7,100);
	}
	PA_SetSpriteXY(screen,2,67,94);
}
s8 DeleteConfirmationBox(s8 screen)
{
	DrawFileWindow(screen);
	PA_SetSpriteXY(screen,2,256,192);
	//draw the box header
	PA_TextLetterSpacing(-5);
	PA_16cText(screen,69,66,255, 192,"Delete File...", 2,8,15);
	PA_TextLetterSpacing(0);
	
	PA_16cText(screen,97,93,255, 192,"Are you sure?", 2,7,15);
	//yes button
	//horizontal lines
	PA_Draw16cLine(screen,79,116,121,116,1);
	PA_Draw16cLine(screen,79,137,121,137,1);
	//vert lines
	PA_Draw16cLine(screen,79,116,79,137,1);
	PA_Draw16cLine(screen,121,116,121,137,1);
	//text
	PA_16cText(screen,93,123,255, 192,"Yes", 2,7,10);
	//no button
	//horizontal lines
	PA_Draw16cLine(screen,130,116,172,116,1);
	PA_Draw16cLine(screen,130,137,172,137,1);
	//vert lines
	PA_Draw16cLine(screen,130,116,130,137,1);
	PA_Draw16cLine(screen,172,116,172,137,1);
	//text
	PA_16cText(screen,138,123,255, 192,"Cancel", 2,7,10);
	bool boxloop=1;
	PA_WaitForVBL();//update stylus input
	while(boxloop==1)
	{
		if(Stylus.Newpress)
		{
			//yes button
			if(PA_StylusInZone(79,116,121,137))
				return 1;
			//no
			else if(PA_StylusInZone(130,116,172,137))
				return 0;
		}
		PA_WaitForVBL();
	}
	return 0;
}

void ShowFileInfo(s8 screen,s16 filenum, u8 mode)
{
	DrawFileInfoBox(screen, filenum,mode);
	bool BoxLoop=1;
	while(BoxLoop)
	{
		//exit
		if(Stylus.Newpress && PA_StylusInZone(165,64,186,77))
		{
			BoxLoop=0;
			FB_OpenFile=2;
		}

		if(!GET_FLAG(files[filenum].Flags, Is_Folder))
		{
		//files[filenum].Folder){//not a folder
			//open file  or unzip
			if(Stylus.Newpress && PA_StylusInZone(152,132,186,145))
			{
				if(!strcmp (files[filenum].ext,"gz"))
				{//if file is gz, then unzip it
					FB_OpenFile=3;
					BoxLoop=0;			
				}
				else
				{	
					FB_OpenFile=1;
					BoxLoop=0;
				}
			}	
			//delete file
			if(Stylus.Newpress && PA_StylusInZone(64,132,101,145))
			{
				if(DeleteConfirmationBox(screen))
				{
					remove (files[filenum].filename);
					FB_OpenFile=-1;
					BoxLoop=0;
				}
				else 
					DrawFileInfoBox(screen, filenum,mode);
			}
			//zip if file is not a gz
			if(Stylus.Newpress && PA_StylusInZone(101,132,151,145) && strcmp (files[filenum].ext,"gz") )
			{
				FB_OpenFile=3;
				BoxLoop=0;
			}
		}
		else
		{
			//open folder
			if(Stylus.Newpress && PA_StylusInZone(101,132,151,145) )
			{
				FB_OpenFile=1;
				BoxLoop=0;
			}	
		}
		
		PA_WaitForVBL();
	}
	PA_16cClearZone(0,0,0,256,192);
	PA_SetSpriteXY(0,2,256,192);
	PA_SetBgPrio(0,2,3);
	PA_WaitForVBL();
}
		
void UpdateCursor(s32 Scroll, s8 direction)
{
	if(Scroll<0)
		Scroll=0;
	if(!(Scroll>DSBmp.NumOfFiles-1) && !(Scroll>(FB_FilesPerScreen-1)))
	{
		PA_16cClearZone(0,0,0,256,192);
		s8 i=0;
		for(i=0;i<FBScale;i++)
			PA_Draw16cLine(0,0,LargeTextHeight+(FBScale*Scroll)+i+FB_ScrollOffset,235,LargeTextHeight+(FBScale*Scroll)+i+FB_ScrollOffset,3);
	}
}

void CreateFBScrollBar(s8 Screen,s8 sprite, s8 palette, s32 x, s32 y, u16 color)
{
	FBScrollBar.Screen=Screen;
	FBScrollBar.Sprite=sprite;
	FBScrollBar.X=x;
	FBScrollBar.Y=y;
	
	PA_LoadSpritePal(FBScrollBar.Screen,palette,(void*)fbscroll_Pal);	// Palette name
	//sprite 
	PA_CreateSprite(FBScrollBar.Screen,FBScrollBar.Sprite,(void*)fbscroll_Sprite,OBJ_SIZE_8X16,1,palette,FBScrollBar.X,FBScrollBar.Y);
	PA_SetSpritePalCol(FBScrollBar.Screen, FBScrollBar.Sprite,1,color);
}

void FBMessage(const char *message)
{
	DC_FlushRange(PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(0,PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	PA_TextLetterSpacing(-4);
	PA_16bitText(1,60,78, SCREEN_WIDTH,SCREEN_HEIGHT,message,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 6, 1, 100);
	PA_TextLetterSpacing(0);
}

void DrawDirPath(const char* path)
{
	DC_FlushRange(PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(0,PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	PA_16bitText(1,0,0, SCREEN_WIDTH,SCREEN_HEIGHT,path,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 5, 1, 100);
}	

void DrawFBGui(s8 mode)
{
	//draw gui
	PA_16cErase  (1);
	SetFBStyle(FB_Default);
	PA_SetBgRot(0, 3,0,0,0,0,0, 256);
	CreateFBSelectBar(PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue));
	PA_TextLetterSpacing(-5);
	
	sprintf(InfoText,"Objects: %d found", DSBmp.NumOfFiles);
	PA_16bitText(0,0,2,SCREEN_WIDTH,SCREEN_HEIGHT, InfoText, PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 6, 1, 100);

	PA_Draw16bitLineEx (0,0,LargeTextHeight-3,256,LargeTextHeight-3,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),2);
	PA_TextLetterSpacing(0);
	//draw X box
	PA_Draw16bitLineEx (0,235,0,235,192,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),1);
	//draw X
	PA_Draw16bitLineEx (0,241,1,251,11,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue),1);
	PA_Draw16bitLineEx (0,241,11,251,1,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue),1);
	CreateFBScrollBar(0,0,ScrollBarPal, 242,LargeTextHeight,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue));
	
	PA_LoadSpritePal(0,2,(void*)fbicons_Pal);	// Palette name
	PA_CreateSprite(0,2,(void*)fbicons_Sprite,OBJ_SIZE_32X32,1,2,256,192);
	int i=0, z=0,Scale=FBScale;
	for(z=0;z<2;z++)
	{
		if(z>=1)
			Scale=8;//keep the preview small and compact
		PA_LoadSpritePal(z,3,(void*)fbiconssmall_Pal);	// Palette name
		PA_SetSpritePalCol(z, 3,1,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB));
	
	
		for(i=0;i<FB_FilesPerScreen;i++)
		{
			PA_CreateSprite(z,i+3,(void*)fbiconssmall_Sprite,OBJ_SIZE_8X8,1,3,0,LargeTextHeight+(Scale*i));
			if(z==0 && FBScale>=16)
			{
				PA_SetSpriteDblsize(0,i+3, 1);
				PA_SetSpriteRotEnable(0,i+3,0);
				PA_SetRotsetNoAngle(0,0,128, 128); 	
			}
				
			PA_SetSpriteAnimEx (z,i+3,8,8,1,3);
		}
	}
	
}
/*================================================================================================================
File Browser Data Collection
=================================================================================================================*/
extern inline char PA_GetSmall(char letter)
{
	if((letter >= 'A')&&(letter <= 'Z')) 
		return (letter-'A'+'a');
	
   return letter;
}   

void filetolower(char *input,char *output,int size)
 {
	int i=0;
	for(i=0;i<size;i++)
	{
		if (input[i] < 'A' || input[i] > 'Z') 
			output[i]=input[i];
			
		else output[i] = input[i]+ ('a' - 'A');
	}
}
s16 FATSearchExt(DIR_ITER* dir, FileList *filelist,s8 filetypes,...)
{
	s16 number = 0;
	s16 i=0, j=0,skipfile=0,ExtLoop=0;
	struct stat st;
	
	va_list ap;
	va_start (ap, filetypes); 
	//collect all extentions supplied
	char extentions[filetypes+1][6];
	for(i=0;i<filetypes;i++)
		strcpy (extentions[i], va_arg (ap,char*));
		
	va_end (ap);

	while (dirnext(dir, filelist[number].filename, &st) == 0) 
	{
		if(!(st.st_mode & S_IFDIR))
		{// Is a file...
			// Check if right extension...
			skipfile=0;
			//filelist[number].Folder=0;
			RESET_FLAG(filelist[number].Flags, Is_Folder);
			i = 0;
			while( filelist[number].filename[i]) 
				i++; // go to end of name
			while( filelist[number].filename[i-1] != '.')
			{
				i--; // get to extension
				if(i<0)
				{
					skipfile=1;
					break;//no extention exists
				}
			}
			if(!skipfile)
			{
				u8 same = 1; // same by default
				for(ExtLoop=0;ExtLoop<filetypes;ExtLoop++)
				{
					same = 1; 
					for(j = 0; extentions[ExtLoop][j]; j++) 
						if(PA_GetSmall(extentions[ExtLoop][j]) !=  PA_GetSmall(filelist[number].filename[i+j])) 
							same = 0; // not right extension
					if(same==1)
						break;
				}
			
				if(same>0)
				{ // Ok, register file	
					for(i = 0; extentions[ExtLoop][i]; i++) 
						filelist[number].ext[i] =extentions[ExtLoop][i];
						
					filelist[number].ext[i] = 0;
					number++;
				}  
			}
	   
		}
		else if(st.st_mode & S_IFDIR)
		{ //is a folder
			if(strcmp(filelist[number].filename,"."))
			{
				SET_FLAG(filelist[number].Flags, Is_Folder);
				number++;
			}
		}
		
		if(number>Settings.MaxFiles)
			return number;
	}
	
	return number;
}

s16 FATSearchExtSmall(DIR_ITER* dir, AudioList *filelist, char *ext)
{
	s16 number = 0;
	u8 i=0, j=0;
	struct stat st;
	while (dirnext(dir, filelist[number].filename, &st) == 0) 
	{
		if(!(st.st_mode & S_IFDIR))
		{// Is a file...
			// Check if right extension...
			i = 0;
			while( filelist[number].filename[i]) 
				i++; // go to end of name
			while( filelist[number].filename[i-1] != '.') 
				i--; // get to extension
			
			u8 same = 1; // same by default
			for(j = 0; ext[j]; j++) 
				if(PA_GetSmall(ext[j]) !=  PA_GetSmall(filelist[number].filename[i+j])) 
					same = 0; // not right extension
			if(same==0)
				same =2;
					
			if(same!=0)
			{ // Ok, register file
				number++;
				if(number>Settings.MaxFiles)return number;
			}  	
		}   
	}
	return number;
}
s16 PreviewDir(DIR_ITER* dir,s8 filetypes,...)
{
	s16 number = 0;
	s16 i=0, j=0,ExtLoop=0;
	s8 skipfile;
	struct stat st;
	
	va_list ap;
	va_start (ap, filetypes); 
	//collect all extentions supplied
	char extentions[filetypes+1][6];
	for(i=0;i<filetypes;i++)
		strcpy (extentions[i], va_arg (ap,char*));
	va_end (ap);	
	
	char TempFile[24][256];
	
	while (number<22 && dirnext(dir,TempFile[number], &st) == 0) 
	{
		if(!(st.st_mode & S_IFDIR))
		{// Is a file...
			// Check if right extension...
			i = 0;
			skipfile=0;
			while( TempFile[number][i])
				i++; // go to end of name
			while( TempFile[number][i-1] != '.') 
			{
				i--; // get to extension
				if(i<0)
				{
					skipfile=1;
					break;
				}
			}
			if(!skipfile)
			{
				u8 same = 1; // same by default
				for(ExtLoop=0;ExtLoop<filetypes;ExtLoop++)
				{
					same = 1; 
					for(j = 0; extentions[ExtLoop][j]; j++) 
						if(PA_GetSmall(extentions[ExtLoop][j]) !=  PA_GetSmall( TempFile[number][i+j])) 
							same = 0; // not right extension
					if(same==1)
						break;
				}
				
				if(same>0)
				{ // Ok, register file
					PA_16bitText(1,9,LargeTextHeight+(TextHeight*number), 234,192,TempFile[number], PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
					if(!strcmp("bmp",extentions[ExtLoop]) ||!strcmp("epf",extentions[ExtLoop]))
						PA_SetSpriteAnimEx (1,number+3,8,8,1,2); 
					else if(!strcmp("ttf",extentions[ExtLoop]))
						PA_SetSpriteAnimEx (1,number+3,8,8,1,4); 
					else if(!strcmp("gz",extentions[ExtLoop]))
						PA_SetSpriteAnimEx (1,number+3,8,8,1,5); 
					number++;
				}  		
			}
		}
		else if(st.st_mode & S_IFDIR)
		{ //is a folder
			if(strcmp(TempFile[number],".") && strcmp(TempFile[number],".."))
			{
				PA_16bitText(1,9,LargeTextHeight+(TextHeight*number), 234,192,TempFile[number],PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 5, 1, 100);
				PA_SetSpriteAnimEx (1,number+3,8,8,1,1); 
				number++;
			}
		}
	}
	return number;
}
void SortFileAlpha(FileList * filelist, int files)
{
	if(files>1)
	{
		int x=0,y=0;
		FileList temp;
		char TempName[2][128];
		for(x=0; x<files; x++)
		{
			for(y=0; y<files-1; y++)
			{
				//convert both strings to lowercase for comparison
				filetolower(filelist[y].filename,TempName[0],127);
				filetolower(filelist[y+1].filename,TempName[1],127);			
				if(strncmp (TempName[0],TempName[1],127)>0)
				{
					//full filename
					temp = filelist[y+1];
					filelist[y+1] = filelist[y];
					filelist[y] = temp;
				}
				if(GET_FLAG(filelist[y+1].Flags, Is_Folder))
				{
					//full filename
					temp = filelist[y];
					filelist[y] = filelist[y+1];
					filelist[y+1] = temp;
				}	
				if(GET_FLAG(filelist[y].Flags, Is_Folder) && GET_FLAG(filelist[y+1].Flags, Is_Folder))
				{
					//full filename
					temp = filelist[y];
					filelist[y] = filelist[y+1];
					filelist[y+1] = temp;
				}	
			}

		}
	}
}

void ScanFileSizes(int filenumber,s8 mode )
{
	//if file is bmp
	char LoadingFile[256];
	FILE *bgfile=NULL;
	switch(mode)
	{
		case FB_Image:
			sprintf(LoadingFile,"%s%s",Settings.imagepath,files[filenumber].filename);
			bgfile = fopen (LoadingFile, "rb");
			if(bgfile)
			{
				if(!strcmp (files[filenumber].ext,"bmp"))
				{
					//read the width
					fseek(bgfile, 18, SEEK_SET);
					fread(&FileInfo.Width, sizeof(int),1, bgfile);
					//read the height
					fseek(bgfile, 22, SEEK_SET);
					fread(&FileInfo.Height, sizeof(int), 1, bgfile);
					//bits
					fseek(bgfile, 28, SEEK_SET);
					fread(&FileInfo.Bits, sizeof(short), 1, bgfile);
					fclose(bgfile);
				}
				if(!strcmp (files[filenumber].ext,"epf"))
				{
					fseek(bgfile, 4, SEEK_SET);
					fread(&FileInfo.Bits,1, 1, bgfile);
					fseek(bgfile, 6, SEEK_SET);
					fread(&FileInfo.Width,2,1, bgfile);
					fread(&FileInfo.Height,2,1, bgfile);	
					fclose(bgfile);
				}
				if(!strcmp (files[filenumber].ext,"gz"))
				{
				
					int UncompressedSize=0,CompressedSize=0;
				
					fseek (bgfile , 0 , SEEK_END);
					
					CompressedSize=ftell(bgfile);
					
					fseek (bgfile , -4 , SEEK_CUR);
					fread (&UncompressedSize,4, 1,bgfile);
					rewind (bgfile); 
					fclose(bgfile);		

					FileInfo.UnZipped = UncompressedSize;
					FileInfo.Size = CompressedSize;
				}
			}
		break;
		case FB_IntFont:
		case FB_Font:
			sprintf(LoadingFile,"%s%s",Settings.fontpath,files[filenumber].filename);
			bgfile = fopen (LoadingFile, "rb");
			if(bgfile)
			{
				if(!strcmp (files[filenumber].ext,"ttf"))
				{
					fseek (bgfile, 0 , SEEK_END);
					FileInfo.Size = ftell (bgfile);
					rewind (bgfile);
					fclose (bgfile);
				}
			}
		break;
	}
}
//========================================================================================================================================================
//audio file listing
//========================================================================================================================================================
#ifdef ENABLEOGG
void PlayOgg(const char* folder,s32 track)
{//play song number from folder
	//clear previous lists and folders
	if(Settings.EnableAudio)
	{
		DIR_ITER* dir =NULL;
		if(Audio!=NULL)
		{
			free(Audio);
			Audio = NULL;
		}
		Audio= (AudioList *)calloc(Settings.MaxFiles,sizeof(AudioList));
		if(Audio==NULL)
			 BAG_Error_Print(Error_OggListAlloc);
		
		dir = diropen(folder);
		DSBmp.NumOfFiles= FATSearchExtSmall(dir,Audio,"ogg");
		Ogg_MaxTracks=DSBmp.NumOfFiles;
		dirclose (dir);
		
		char AudioFile[256];
		memset(AudioFile,0,256);
		if(track>=DSBmp.NumOfFiles)
			track=DSBmp.NumOfFiles-1;
		else if(track<0)
		{//random song
			track= rand() % (DSBmp.NumOfFiles-1);
			Ogg_Track=track;
		}
		
		strcpy (Ogg_CurrentSong,Audio[track].filename);
		sprintf(AudioFile,"%s%s",folder,Audio[track].filename);
		//clear file list
		free(Audio);
		Audio = NULL;
		
		Play_OGG(AudioFile);
	}
}
void UpdateOgg(void)
{
	//debug
	UserDumpDebug();
	UpdateTB();
	s8 i=0;
	for(i=0;i<2;i++)
		FadeInterface(Fade.Style[i],Fade.ColWheel[i]);
	if(Settings.EnableAudio &&OggStatus==1&& Ogg_AutoPlay==1)
		PlayOgg(Settings.audiopath,-1);
	//mmStreamUpdate();
}
#endif
/*================================================================================================================
File Browser Code
=================================================================================================================*/
void ShowPreview(int file,s8 mode)
{
	//clear directory preview sprites
	int i=0;
	for(i=0;i<22;i++)
		PA_SetSpriteAnimEx (1,i+3,8,8,1,3); 
	
	ScanFileSizes(file,mode);
	if(file<DSBmp.NumOfFiles)
	{
		char LoadingFile[256];
		memset(LoadingFile,0,256);
		//directory browsing stuff
		char CurrentPath[256], fullpath[256];	

		if(!GET_FLAG(files[file].Flags, Is_Folder))
		{
			switch(mode)
			{
				case FB_Image:
					sprintf(LoadingFile,"%s%s",Settings.imagepath,files[file].filename);
					if(!strcmp (files[file].ext,"bmp"))
					{
						s8 status=LoadBmpPreview(LoadingFile);
						if(status==-1)
						{
							FBMessage("Image too large");
							SET_FLAG(files[file].Flags, Not_Supported);
						}
						else if(status==-2)
							FBMessage("Not enough memory\nTo display");
					}
					else if(!strcmp (files[file].ext,"epf"))
					{
						s8 status=LoadEpfPreview(LoadingFile);
						if(status==-1)
						{
							FBMessage("Image too large");
							SET_FLAG(files[file].Flags, Not_Supported);
						}
						else if(status==-2)
							FBMessage("Not enough memory\nTo display");
					}
					else 
						FBMessage("No Preview Available");
				break;
				case FB_IntFont:
				case FB_Font:
					sprintf(LoadingFile,"%s%s",Settings.fontpath,files[file].filename);
					ScanFileSizes(file,FB_Font);
					if(!strcmp (files[file].ext,"ttf"))
					{	
						if(FileInfo.Size>Settings.FontFileSize)
						{
							FBMessage("Font too large");
							SET_FLAG(files[file].Flags, Not_Supported);
						}
						else if(!BAG_FontFatLoad(LoadingFile,0,1))
						{
							SET_FLAG(files[file].Flags, Not_Supported);
							FBMessage("Failed to Load");
						}
					}
					else 
						FBMessage("No Preview Available");
				break;
			}
		}
		else
		{
			getcwd(CurrentPath,256);
			sprintf(fullpath, "%s%s",CurrentPath,files[file].filename);
			DrawDirPath(fullpath);
			PA_Draw16bitLineEx (1,0,LargeTextHeight-3,256,LargeTextHeight-3,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),2);
			DIR_ITER* dir = diropen(fullpath);
			
			switch(mode)
			{
				case FB_Image:
					DSBmp.PreviewFolderCount = PreviewDir(dir,3,"bmp","epf","gz");
				break;
				case FB_IntFont:	
				case FB_Font:
					DSBmp.PreviewFolderCount = PreviewDir(dir,2,"ttf","gz");
				break;
			}
			dirclose (dir);
		}
	}
}

void ChangeBrowseDir(char* currentDir,s8 mode,s16 scroll)
{
	
	chdir(currentDir);	//clear the folder struct
	char CurrentDirectory[256];
	getcwd(CurrentDirectory,256);
	DIR_ITER* dir = diropen(CurrentDirectory);
	//check if files struct is filled, if so clear it
	if(files!=NULL)
	{
		free(files);
		files = NULL;
	}
	files= (FileList *)calloc(Settings.MaxFiles,sizeof(FileList));
	if(files==NULL)
		BAG_Error_Print(Error_BrowserAlloc);

	//sprintf(Settings.folderpath,Settings.imagepath);
	//scan for different files and folders
	switch(mode)
	{
		case FB_Image:
			strcpy(Settings.imagepath,CurrentDirectory);
			DSBmp.NumOfFiles= FATSearchExt(dir,files,3,"bmp","epf","gz");
		break;
		case FB_IntFont:
		case FB_Font:
			strcpy(Settings.fontpath,CurrentDirectory);
			DSBmp.NumOfFiles= FATSearchExt(dir,files,2,"ttf","gz");	
		break;	
	}
	SortFileAlpha(files,DSBmp.NumOfFiles);
	
	dirclose (dir);
	ClearScreen(0);
	DrawFBGui(mode);

	//list text
	ScrollFBText(scroll,mode);
	
	Stylus.oldy[0]=192;
}

s8 LoadFBFile(s16 filenum, s8 mode)
{
	char LoadingFile[256];
	memset(LoadingFile,0,256);
	if(!GET_FLAG(files[filenum].Flags, Is_Folder))
	{
		switch(mode)
		{
			case FB_Image:
				if(!GET_FLAG(files[filenum].Flags, Not_Supported))
				{
					sprintf(LoadingFile,"%s%s",Settings.imagepath,files[filenum].filename);
					if(!strcmp (files[filenum].ext,"bmp"))
					{
						if(!LoadBmp(LoadingFile))
						{
							BAG_Error_Print(Error_BmpLoad);
							return -1;
						}
					}
					else if(!strcmp (files[filenum].ext,"epf"))
					{
						if(!LoadEPF(LoadingFile))
						{
							BAG_Error_Print(Error_EpfLoad);
							return -1;
						}
					}
					else if(!strcmp (files[filenum].ext,"gz"))
						return 3;
					return 1;	
				}
			break;
			case FB_IntFont:
			case FB_Font:
				if(!GET_FLAG(files[filenum].Flags, Not_Supported))
				{
					if(!strcmp (files[filenum].ext,"ttf"))
					{
						sprintf(LastFont,"%s%s",Settings.fontpath,files[filenum].filename);
						return 1;
					}
					else if(!strcmp (files[filenum].ext,"gz"))
						return 3;
				}
			break;
		}
	}
	else
	{
		ChangeBrowseDir(files[filenum].filename,mode,0);
		ShowPreview(0,mode);
		UpdateCursor(0,1);
		return 2;
	}
	return 0;
}
void ShowCurrentDir(s8 mode,s16 scroll)
{
	DIR_ITER* dir =NULL;
	switch(mode)
	{
		case FB_Image:
			dir = diropen(Settings.imagepath);
			if(dir==NULL)
			{
				#ifdef AllowBrowseNitro
					sprintf(Settings.imagepath,"nitro:/pics/");
				#else 
					sprintf(Settings.imagepath,"FAT:/");
				#endif
			}				
			else 
				dirclose (dir);
			ChangeBrowseDir(Settings.imagepath,mode,scroll);
		break;
		case FB_IntFont:
		case FB_Font:
			dir = diropen(Settings.fontpath);
			if(dir==NULL)
			{
				#ifdef AllowBrowseNitro
					sprintf(Settings.fontpath,"nitro:/_fonts/");
				#else
					sprintf(Settings.imagepath,"FAT:/");
				#endif
			}				
			else 
				dirclose (dir);
			ChangeBrowseDir(Settings.fontpath,mode,scroll);
		break;
	}
}
void ShowFileBrowser(s8 mode)
{

	s8 RunFileBrowser=1;
	s32 StylusFile=0,oldfile=0,ListScroll=0,ScreenScroll=0, padTimer=0;
	int OldScreenScroll=-1;
	if(mode == FB_Image)
		DSBmp.PrgmLocation = Location_ImgBrowser;
	else 
		DSBmp.PrgmLocation = Location_FontBrowser;
	
	ClearScreen(1);
	ClearScreen(0);
	ShowCurrentDir(mode,0);
	UpdateCursor(0,1);
	ShowPreview(0,mode);
	if(ListScroll<ScreenScroll)ScreenScroll=ListScroll;
	FB_OpenFile=0;
	s16 HeldDif=0,TempScrollBarY=0;
	
	s16 ScaledStylusY = ( Stylus.Y - (FB_ScrollOffset<<2)) / FBScale;
	s16 ScaledStylusOldY = (Stylus.oldy[0]- (FB_ScrollOffset<<2)) / FBScale;
	while(RunFileBrowser==1)
	{
		//dpad scrolling
		if(Pad.Held.Down || Pad.Held.Up)
		{
			if(padTimer<20)padTimer++;
		}
		if(Pad.Newpress.Down || (Pad.Held.Down && padTimer>=20))
		{
			if(ListScroll + ScreenScroll<DSBmp.NumOfFiles-1) 
			{
				ListScroll++;
				if(ListScroll>(FB_FilesPerScreen-1))
				{
					ScreenScroll++;
					s32 NewScrollPos = ((ScreenScroll*160)/(DSBmp.NumOfFiles-1-(FB_FilesPerScreen-1)))+(16);
					PA_SetSpriteXY(FBScrollBar.Screen,FBScrollBar.Sprite,FBScrollBar.X,NewScrollPos);
					TempScrollBarY=NewScrollPos;
					ScrollFBText(ScreenScroll,mode);
					ListScroll=(FB_FilesPerScreen-1);
					UpdateCursor(ListScroll,1);
				}
				else 
					UpdateCursor(ListScroll,0);
				StylusFile=ListScroll+ScreenScroll;
			}
		}

		if(Pad.Newpress.Up|| (Pad.Held.Up && padTimer>=20))
		{
			if(ListScroll + ScreenScroll>0)
			{
				ListScroll--;
				if(ListScroll<0)
				{
					ScreenScroll--;
					s32 NewScrollPos = ((ScreenScroll*160)/(DSBmp.NumOfFiles-1-(FB_FilesPerScreen-1)))+(16);
					PA_SetSpriteXY(FBScrollBar.Screen,FBScrollBar.Sprite,FBScrollBar.X,NewScrollPos);
					TempScrollBarY=NewScrollPos;
					ScrollFBText(ScreenScroll,mode);
					ListScroll=0;
					UpdateCursor(ListScroll,1);
				}
				else 
					UpdateCursor(ListScroll,1);
				StylusFile=ListScroll+ScreenScroll;
			}
		}
		if(!Pad.Held.Up && !Pad.Held.Down && !Stylus.Held )
		{
			if(StylusFile!=oldfile)
				ShowPreview(StylusFile,mode);
			oldfile=StylusFile;
			padTimer=0;
			HeldDif=0;
		}
		
		if(Stylus.Newpress)
		{
			//exit
			if(PA_StylusInZone(235,0,256,14))
			{
				free(files);
				files = NULL;
				CalculateScale(DSBmp.Width,DSBmp.Height,Settings.MiniviewWidth,Settings.MiniviewHeight);
				RunFileBrowser=0;
			}
			//scrollbar, update the screenscroll and old scroll
			if(PA_StylusInZone(FBScrollBar.X-4,FBScrollBar.Y,FBScrollBar.X+16,FBScrollBar.Y+160))
			{
				ScreenScroll = ((Stylus.Y-LargeTextHeight)*(DSBmp.NumOfFiles-(FB_FilesPerScreen-1)))/160;
				OldScreenScroll=ScreenScroll;
			}
		
			while(Stylus.Held && !FB_OpenFile)
			{
				ScaledStylusY = ( Stylus.Y - (FB_ScrollOffset<<2)) / FBScale;
				 ScaledStylusOldY = (Stylus.oldy[0]- (FB_ScrollOffset<<2)) / FBScale;
				 
				if(DSBmp.NumOfFiles>(FB_FilesPerScreen-1))
				{
					if(PA_StylusInZone(FBScrollBar.X-4,FBScrollBar.Y,FBScrollBar.X+16,FBScrollBar.Y+160))
					{
						PA_SetSpriteXY(FBScrollBar.Screen,FBScrollBar.Sprite,FBScrollBar.X,Stylus.Y);
						TempScrollBarY=((ScreenScroll*160)/(DSBmp.NumOfFiles-1-(FB_FilesPerScreen-1)))+(16);
						ScreenScroll = ((Stylus.Y-LargeTextHeight)*(DSBmp.NumOfFiles-(FB_FilesPerScreen-1)))/160;
						
						if(ScreenScroll!=OldScreenScroll)
						{
							if(ScreenScroll>OldScreenScroll)
								HeldDif--;
							else 
								HeldDif++;
							UpdateCursor(ListScroll+HeldDif,1);
							ScrollFBText(ScreenScroll,mode);
							OldScreenScroll=ScreenScroll;
						}
					}
				}
				if(PA_StylusInZone(0,LargeTextHeight,234,192))
				{
					if(DSBmp.NumOfFiles<(FB_FilesPerScreen-1))
					{
						if(ScaledStylusY-2<DSBmp.NumOfFiles)
						{
							StylusFile =ScaledStylusY-2 + ScreenScroll;
							ListScroll=ScaledStylusY-2;
						}
						else 
						{
							StylusFile =DSBmp.NumOfFiles-1;
							ListScroll = DSBmp.NumOfFiles-1;
						}
					}
					else 
					{
						StylusFile =ScaledStylusY-2 + ScreenScroll;
						ListScroll=ScaledStylusY-2;
					}
					if(ScaledStylusY!=ScaledStylusOldY)
					{
						if(ScaledStylusY<ScaledStylusOldY)
							UpdateCursor(ScaledStylusY-2,1);
						else 
							UpdateCursor(ScaledStylusY-2 ,0);
					}
					Stylus.oldy[0]=Stylus.Y;
				}
				PA_WaitForVBL();
			}
		}

		//Load file
		if(Pad.Newpress.A || FB_OpenFile==1)
		{
			char Output[256];
			switch(LoadFBFile(StylusFile,mode))
			{
				default:break;
				case -1:
					PA_16bitText(0,0,176, 255,192, "failed to load", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);
					FB_OpenFile=0;
				break;
				case 0:
					//nothing
					FB_OpenFile=0;
				break;
				case 1:
					RunFileBrowser=2;
					FB_OpenFile=0;
					break;
				break;
				case 2://opened a folder
					StylusFile=0,oldfile=0,ListScroll=0,ScreenScroll=0, padTimer=0;FB_OpenFile=0;
				break;
				case 3:
					snprintf(Output,strlen(files[StylusFile].filename)-2,"%s",files[StylusFile].filename);
					dsUncompress(files[StylusFile].filename ,Output);
					FB_OpenFile=0;	
					ShowCurrentDir(mode,ScreenScroll);
					UpdateCursor(ListScroll,1);		
				break;
			}	
		}
		if(Pad.Newpress.Y)
		{
			ShowFileInfo(0,StylusFile,mode);
			if(FB_OpenFile==-1)
			{//file was deleted so update the file list
				FB_OpenFile=0;
				if(ScreenScroll>0)
					ScreenScroll--;
				if(StylusFile>0)
					StylusFile--;
				ShowCurrentDir(mode,ScreenScroll);
				TempScrollBarY=((ScreenScroll*160)/(DSBmp.NumOfFiles-1-(FB_FilesPerScreen-1)))+(16);
				PA_SetSpriteXY(FBScrollBar.Screen,FBScrollBar.Sprite,FBScrollBar.X,TempScrollBarY);
				UpdateCursor(ListScroll,0);
				ShowPreview(StylusFile,mode);
			}
			//zip or unzip file
			if(FB_OpenFile==3)
			{
				//if file is a gzip file
				if(!strcmp (files[StylusFile].ext,"gz"))
				{
					char Output[256];
					snprintf(Output,strlen(files[StylusFile].filename)-2,"%s",files[StylusFile].filename);
					dsUncompress(files[StylusFile].filename ,Output);	
				}
				else
				{//not gzip so we can zip it
					char Output[256];
					sprintf(Output,"%s.gz",files[StylusFile].filename);
					dsCompress(files[StylusFile].filename,Output);
				}
				ShowCurrentDir(mode,ScreenScroll);
				UpdateCursor(ListScroll,1);		
				FB_OpenFile=0;
			}
			else
			{ 
				TempScrollBarY=((ScreenScroll*160)/(DSBmp.NumOfFiles-1-(FB_FilesPerScreen-1)))+(16);
				PA_SetSpriteXY(FBScrollBar.Screen,FBScrollBar.Sprite,FBScrollBar.X,TempScrollBarY);
				ScrollFBText(ScreenScroll,mode);
				UpdateCursor(ListScroll,0);
			}
			if(FB_OpenFile==2)
				FB_OpenFile=0;
		}

			
		PA_WaitForVBL();
	}
	PA_SetBgRot(0, 3,0,0,0,0,0, Draw.Zoom);
	free(files);
	files = NULL;
	switch(mode)
	{
		case FB_Image:
			LoadMainInterFace();
		break;
		case FB_IntFont:
			if(RunFileBrowser==0){
				Font_FatUnload(0);
				LoadMainInterFace();
				break;
			}
			else mode++;
		case FB_Font:
			BAG_FontFatLoad(LastFont,0,1);
			CreateKBMenu();
		break;
	}
}
