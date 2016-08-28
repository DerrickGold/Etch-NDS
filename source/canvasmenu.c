#include <PA9.h>
#include "BAGKeyboard.h"
#include "bgdata.h"

void DrawInputBoxHighlight(s32 x,s32 y)
{
	int x1=0,y1=0;
	//top && bottom
	for(x1=x;x1<x+56;x1++)
	{
		PA_Put16bitPixel(0,x1,y,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue));
		PA_Put16bitPixel(0,x1,y+8,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue));
	}
	for(y1=y;y1<=y+8;y1++)
	{
		PA_Put16bitPixel(0,x,y1,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue));
		PA_Put16bitPixel(0,x+56,y1,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue));
	}		
}
void DrawDimensionBoxs(s8 Entry, int width, int height)
{
	int x=0,y=0;
	char Buf[2][8];
	sprintf(Buf[0],"%d",width);
	sprintf(Buf[1],"%d",height);
	//Dimension boxes
	switch(Entry){
		case 0:
			for(x=151;x<151+57;x++)
				for(y=34;y<34+9;y++)PA_Put16bitPixel(0,x,y,PA_RGB(31,31,31));
			PA_16bitText(0,155,35, 151+(16*3),192, Buf[0], PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
		break;
		case 1:
			for(x=151;x<151+57;x++)
				for(y=64;y<64+9;y++)PA_Put16bitPixel(0,x,y,PA_RGB(31,31,31));	
			PA_16bitText(0,155,65, 151+(16*3),192, Buf[1], PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
		break;
	}
}
void RedrawCanvasInfo(int width, int height)
{
	int x=0,y=0;

	for(x=22;x<121;x++)
	{
		for(y=42;y<88;y++)
			PA_Put16bitPixel(0,x,y,0);
	}
	//text for info box
	sprintf(InfoText,"Layers Available: %d/%d",DSBmp.NumOfLayers,Settings.MaxLayers);
	PA_16bitText(0,22,42, 256,192,InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	sprintf(InfoText,"Undos Available: %d/%d",DSBmp.NumOfUndos,Settings.MaxBuffers);
	PA_16bitText(0,22,54, 256,192,InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);

	//pixel limit text
	PA_16bitText(0,22,68, 256,192,"Pixel Count:", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	sprintf(InfoText,"%d/%d",width*height,Settings.MaxBmpSize);
	
	if(width*height<= Settings.MaxBmpSize && width*height > 0 )//green for good size
		PA_16bitText(0,22,80, 256,192,InfoText, PA_RGB(0,20,0), 5, 1, 100);
	else//red for over limit 
		PA_16bitText(0,22,80, 256,192,InfoText, PA_RGB(31,0,0), 5, 1, 100);
		
}
void DrawCanvasMenu(void)
{
	PA_SetBgRot(0, 3,0,0,0,0,0, 256);
	
	SetKeyBoardScreen(0);
	SetKeyBoardBgMode(KB_BG_16bit);
	SetKeyBoardFont(1);
	SetKeyBoardColors(Settings.KeyboardCol[2],Settings.KeyboardCol[0],Settings.KeyboardCol[3],Settings.KeyboardCol[1]);
	BAG_InitKeyPad(155, 79,16,8);
	
	//screen name
	PA_TextLetterSpacing(-5);
	PA_16bitText(0,0,2, 255,192,"Canvas Info:", PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 6, 1, 100);
	PA_TextLetterSpacing(0);
	PA_Draw16bitLineEx (0,235,0,235,14,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),1);
	//draw X
	PA_Draw16bitLineEx (0,241,1,251,11,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue),1);
	PA_Draw16bitLineEx (0,241,11,251,1,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue),1);	
	PA_Draw16bitLineEx (0,0,LargeTextHeight-3,256,LargeTextHeight-3,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),2);
	
	PA_16bitText(0,162,26, 155+(16*3),192,"Width", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	PA_16bitText(0,162,56, 155+(16*3),192,"Height", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	
	//draw info box
	PA_Draw16bitLineEx (0,19,37,121,37,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),1);//top
	PA_Draw16bitLineEx (0,19,37,19,137,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),1);//left
	PA_Draw16bitLineEx (0,121,37,121,137,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),1);//right
	//confirm button
	PA_Draw16bitLineEx (0,19,106,121,106,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),1);//top
	PA_Draw16bitLineEx (0,19,137,121,137,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),1);//bottom
	PA_Draw16bitLineEx (0,19,106,19,137,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),1);//left
	PA_Draw16bitLineEx (0,121,106,121,137,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),1);//right
	PA_16bitText(0,51,118, 256,192,"Confirm", PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 5, 1, 100);

	//bit depth
	PA_Draw16bitLineEx (0,0,160,256,160,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),1);//top
	PA_16bitText(0,15,174, 256,192,"Bit Mode:", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	PA_16bitText(0,92,174, 256,192,"24 bit", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	PA_16bitText(0,144,174, 256,192,"16 bit", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	PA_16bitText(0,193,174, 256,192,"8 bit", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	
	//check boxes
	PA_LoadSpritePal(0,0,(void*)checkbox_Pal);
	PA_CreateSprite(0,0,(void*)checkbox_Sprite, OBJ_SIZE_16X16,1, 0,117,170);//24 bit
	PA_CreateSprite(0,1,(void*)checkbox_Sprite, OBJ_SIZE_16X16,1, 0,167,170);//16 bit
	PA_CreateSprite(0,2,(void*)checkbox_Sprite, OBJ_SIZE_16X16,1, 0,213,170);//8 bit
}

void NewCanvasLoadScreen(void)
{
	DC_FlushRange(PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	dmaFillHalfWords(0,PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	PA_ResetSpriteSysScreen(0);
	PA_TextLetterSpacing(-4);
	PA_16bitText(0,60,78-16, SCREEN_WIDTH,SCREEN_HEIGHT,"Preparing Canvas",PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 6, 1, 100);
	PA_TextLetterSpacing(0);
	LoadStylusCursor(0,3,1,TB_Bucket,(256>>1)-(16>>1),(192>>1)-(16>>1));
}
void CanvasMenu(void)
{
	DSBmp.PrgmLocation = Location_CanvasSize;
	ClearScreen(0);
	DrawCanvasMenu();
	
	bool menu=1;
	u32 TempWidth=Settings.DefaultX,TempHeight=Settings.DefaultY;
	s8 EntrySelect=0;
	int i[2];
	char newletter=0;
	//for converting dimensions to integers
	char DimensionBuf[2][8];
	sprintf(DimensionBuf[0],"%d",Settings.DefaultX);
	i[0] = strlen(DimensionBuf[0]);
	sprintf(DimensionBuf[1],"%d",Settings.DefaultY);
	i[1] = strlen(DimensionBuf[1]);
	
	DrawDimensionBoxs(0,TempWidth,TempHeight);
	DrawDimensionBoxs(1,TempWidth,TempHeight);
	DrawInputBoxHighlight(151,34);
	CalcAvailableLayers(TempWidth,TempHeight);
	RedrawCanvasInfo(Settings.DefaultX,Settings.DefaultY);
	BitModeCheckBox(DSBmp.Bits);
	while (menu==1)
	{
	
		if(Stylus.Newpress)
		{
			//check if any keys on the numpad are pressed
			newletter=CheckKeyPad();
			if(newletter!=BACKSPACE )
			{
				//prevents i from increasing if 0 is set as the first digit
				if(i[EntrySelect]<7 && newletter!=NO_KEY)
				{
					DimensionBuf[EntrySelect][i[EntrySelect]]=newletter;
					i[EntrySelect]++;
				}
				TempWidth=atoi(DimensionBuf[0]);
				TempHeight=atoi(DimensionBuf[1]);
				DrawDimensionBoxs(EntrySelect,TempWidth,TempHeight);
				DrawInputBoxHighlight(151,34+(30*EntrySelect));
			}
			else if(newletter==BACKSPACE)
			{
				if(i[EntrySelect]>0)i[EntrySelect]--;
				DimensionBuf[EntrySelect][i[EntrySelect]]='\0';
				TempWidth=atoi(DimensionBuf[0]);
				TempHeight=atoi(DimensionBuf[1]);
				DrawDimensionBoxs(EntrySelect,TempWidth,TempHeight);
				DrawInputBoxHighlight(151,34+(30*EntrySelect));
			}
			//width box
			if(PA_StylusInZone(151,34,162+(16*3),48))
			{
				EntrySelect=0;
				i[EntrySelect] = strlen(DimensionBuf[EntrySelect]);
				DrawInputBoxHighlight(151,34);
				DrawDimensionBoxs(1,TempWidth,TempHeight);
			}
			//height box
			else if(PA_StylusInZone(151,64,162+(16*3),78))
			{
				EntrySelect=1;
				i[EntrySelect] = strlen(DimensionBuf[EntrySelect]);
				DrawInputBoxHighlight(151,64);
				DrawDimensionBoxs(0,TempWidth,TempHeight);
			}
			//confirm
			if(PA_StylusInZone(19,106,121,137))
				if(TempWidth*TempHeight>0 && TempWidth*TempHeight<=Settings.MaxBmpSize)menu=2;
				
			//change the bit mode
			if(PA_SpriteTouched(0))
			{
				DSBmp.Bits=24;
				BitModeCheckBox(DSBmp.Bits);
				CalcAvailableLayers(TempWidth,TempHeight);
				RedrawCanvasInfo(TempWidth,TempHeight);
			}
			if(PA_SpriteTouched(1))
			{
				DSBmp.Bits=16;
				BitModeCheckBox(DSBmp.Bits);
				CalcAvailableLayers(TempWidth,TempHeight);
				RedrawCanvasInfo(TempWidth,TempHeight);
			}
			if(PA_SpriteTouched(2))
			{
				DSBmp.Bits=8;
				BitModeCheckBox(DSBmp.Bits);
				CalcAvailableLayers(TempWidth,TempHeight);
				RedrawCanvasInfo(TempWidth,TempHeight);
			}
		}
		if(Stylus.Released)
		{
			if(newletter!='~')
			{ 
				RefreshKeyPad();
				CalcAvailableLayers(TempWidth,TempHeight);
				RedrawCanvasInfo(TempWidth,TempHeight);
			}
		}		
		PA_WaitForVBL();
	}
	if(menu==2)
	{
		#ifdef ENABLEOGG
			s8 tempogg=0;
			if(Settings.EnableAudio)
			{
				tempogg=OggStatus;
				Ogg_Pause(1);
			}
		#endif
		if(Settings.ExternalRam)NewCanvasLoadScreen();
		if(New_Canvas(TempWidth,TempHeight))
		{
			if(Settings.ExternalRam)DeleteCursor();
			#ifdef ENABLEOGG
				if(Settings.EnableAudio)
				{
					if(tempogg==2)
						Ogg_Pause(0);
				}
			#endif
			LoadMainInterFace();
		}
	}
}
