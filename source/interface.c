#include <PA9.h>
#include "BAGKeyboard.h"
#include "bgdata.h"

#define ATTR0_DISABLED   (2<<8) 
void HideSprite(bool screen,int sprite,bool hide){
	switch(hide){
		case 0:PA_obj[screen][sprite].atr0 &= ~ATTR0_DISABLED;break;//show sprite
		case 1:PA_obj[screen][sprite].atr0 |= ATTR0_DISABLED;break;//hide sprite
	}
}

void ClearInterFace(void){
	FastDelTB();
	PA_ResetSpriteSysScreen(0);
	PA_DeleteBg(0,Layer_Menu);
	PA_DeleteBg(0,1);
	//DeleteGrid();
	//PA_DisableSpecialFx(0);
	PA_SetBgPrio(0,Layer_Draw,3);
	PA_SetBgPrio(0,Layer_Menu,2);
}
void SetColorWheelAlpha(s8 alpha){
	s8 i=0;
	for(i=0;i<4;i++){
		PA_Set16bitSpriteAlpha (0,ColorWheel[i].Sprite,alpha);
	}
}

void FadeInterface(s8 InOut, bool cw){
	if(GET_FLAG(Fade.Flags[cw],FADE_IN) && !GET_FLAG(Fade.Flags[cw],FADED_OUT)&&!GET_FLAG(Fade.Flags[cw],FADING_OUT) && !GET_FLAG(Fade.Flags[cw],FADED_IN)){
		if(GET_FLAG(Fade.Flags[cw],FADE_RESET)){
			Fade.Alpha2[cw]=0;Fade.Alpha1[cw]=0;
			RESET_FLAG(Fade.Flags[cw],FADE_RESET);
		}
		if(Fade.Alpha2[cw]<14)Fade.Alpha2[cw]+=4;
		if(Fade.Alpha2[cw]>14)Fade.Alpha2[cw]=14;
		if(Fade.Alpha1[cw]<=16)Fade.Alpha1[cw]+=4;
		if(cw)SetColorWheelAlpha(Fade.Alpha2[cw]);
		PA_SetSFXAlpha(0,Fade.Alpha1[cw],16-Fade.Alpha1[cw]); 
		SET_FLAG(Fade.Flags[cw],FADING_IN);
		
		if(Fade.Alpha2[cw]>=14 && Fade.Alpha1[cw]>=16){
			Fade.Alpha2[cw]=1;Fade.Alpha1[cw]=15;//set values up for fading out
			SET_FLAG(Fade.Flags[cw],FADED_IN);
			RESET_FLAG(Fade.Flags[cw],FADED_OUT);
			RESET_FLAG(Fade.Flags[cw],FADING_IN);
			RESET_FLAG(Fade.Flags[cw],FADE_IN);
		}
	}
	if(GET_FLAG(Fade.Flags[cw],FADE_OUT) && !GET_FLAG(Fade.Flags[cw],FADED_IN) &&!GET_FLAG(Fade.Flags[cw],FADING_IN) && !GET_FLAG(Fade.Flags[cw],FADED_OUT)){
		if(GET_FLAG(Fade.Flags[cw],FADE_RESET)){
			Fade.Alpha2[cw]=1;Fade.Alpha1[cw]=15;
			RESET_FLAG(Fade.Flags[cw],FADE_RESET);
		}
		if(Fade.Alpha2[cw]<=16)Fade.Alpha2[cw]+=5;
		if(Fade.Alpha1[cw]>0)Fade.Alpha1[cw]-=5;
		if(Fade.Alpha1[cw]<0)Fade.Alpha1[cw]=0;
		if(cw)SetColorWheelAlpha(Fade.Alpha1[cw]);
		PA_SetSFXAlpha(0,Fade.Alpha1[cw],Fade.Alpha2[cw]);
		SET_FLAG(Fade.Flags[cw],FADING_OUT);
		if(Fade.Alpha2[cw]>=16 && Fade.Alpha1[cw]<=0){
			Fade.Alpha2[cw]=0;Fade.Alpha1[cw]=0;//set values up for fading in
			SET_FLAG(Fade.Flags[cw],FADED_OUT);
			RESET_FLAG(Fade.Flags[cw],FADED_IN);
			RESET_FLAG(Fade.Flags[cw],FADING_OUT);
			RESET_FLAG(Fade.Flags[cw],FADE_OUT);
		}			
	}
}
void FadeInInterface(bool cw){
	RESET_FLAG(Fade.Flags[cw],FADED_IN);
	RESET_FLAG(Fade.Flags[cw],FADED_OUT);
	SET_FLAG(Fade.Flags[cw],FADE_RESET);
	SET_FLAG(Fade.Flags[cw],FADE_IN);
	Fade.Style[cw] = 0;
	Fade.ColWheel[cw] = cw;
}
void FadeOutInterface(bool cw){
	RESET_FLAG(Fade.Flags[cw],FADED_OUT);
	RESET_FLAG(Fade.Flags[cw],FADED_IN);
	SET_FLAG(Fade.Flags[cw],FADE_RESET);
	SET_FLAG(Fade.Flags[cw],FADE_OUT);
	Fade.Style[cw] = 1;
	Fade.ColWheel[cw] = cw;
}
	
//Stylus Cursor================================================================================================================
//Stylus Cursor================================================================================================================
//Stylus Cursor================================================================================================================
void SetCursorData(s8 screen,s8 palette, s8 sprite,s8 tool, s16 x, s16 y){
	PA_ResetSpriteSysScreen(0);
	PA_DisableSpecialFx(screen);
	Cursor.Type=tool;
	Cursor.X=x;
	Cursor.Y=y;
	Cursor.Sprite=sprite;
	Cursor.Palette=palette;
	Cursor.Screen=screen;
	Cursor.Created=1;
	PA_LoadSpritePal(screen,palette,(void*)Stylus_curs_Pal);	// Palette name
	PA_CreateSprite(screen, sprite, (void*)Stylus_curs_Sprite,OBJ_SIZE_16X16,1, palette,x, y);
}
void DeleteCursor(void){
	if(Cursor.Created){
		PA_DeleteSprite(Cursor.Screen,Cursor.Sprite);
		memset(&Cursor, 0,sizeof(Cursor));//reset cursor struct
	}
}	
	
void LoadStylusCursor(s8 screen,s8 palette, s8 sprite,s8 tool, s16 x, s16 y){
	if(Cursor.Created)DeleteCursor();
	switch(tool){
		default://no cursor
			DeleteCursor();
		break;
		case TB_Bucket://loading cursor
			SetCursorData(screen,palette,sprite,tool,x,y);
			PA_StartSpriteAnim  (screen,sprite,0,6,16);
		break;
		case TB_Move://moving hand
			SetCursorData(screen,palette,sprite,tool,x,y);
			PA_SetSpriteAnim(screen, sprite,7); 
		break;
		case TB_EyeDropper://eye dropper
			SetCursorData(screen,palette,sprite,tool,x,y);
			PA_SetSpriteAnim(screen, sprite,9); 
		break;
	}	
}
void SetCursorXY(s16 x, s16 y){
	if(Cursor.Created){
		Cursor.X=x;
		Cursor.Y=y;	
		PA_SetSpriteXY(Cursor.Screen,Cursor.Sprite,x,y-16);
	}
}
void SetCursorFrame(s8 frame){
	if(Cursor.Created){
		PA_StopSpriteAnim(Cursor.Screen, Cursor.Sprite);
		PA_SetSpriteAnim(Cursor.Screen,Cursor.Sprite,frame); 
	}
}
//Main Interface================================================================================================================
//Main Interface================================================================================================================
//Main Interface================================================================================================================
void LoadMainInterFace(void){
	DSBmp.PrgmLocation = Location_Main;
	ClearInterFace();
	PA_SetBgRot(0, Layer_Draw,DSBmp.HWScrollX,DSBmp.HWScrollY,0,0,0, Draw.Zoom);
	PA_EnableSpecialFx(0,SFX_ALPHA,SFX_OBJ | SFX_BG2 | SFX_BG1 ,SFX_BG0 | SFX_BG3 | SFX_BD); // Everything normal
	PA_EnableSpecialFx(1,SFX_ALPHA,SFX_BG2 ,SFX_OBJ | SFX_BG3 | SFX_BD);
	PA_SetSFXAlpha(1,10,8);		
	PA_SetSFXAlpha(0,0,16);	
	UpdateBothScreens();
	LoadStatsBar();
	ClearSelectBox(0);
	CheckGrid();
}
void FastMainInterFace(void){
	DSBmp.PrgmLocation = Location_Main;
	ClearInterFace();
	PA_SetBgRot(0, Layer_Draw,DSBmp.HWScrollX,DSBmp.HWScrollY,0,0,0, Draw.Zoom);
	PA_EnableSpecialFx(0,SFX_ALPHA,SFX_OBJ | SFX_BG2 | SFX_BG1 ,SFX_BG0 | SFX_BG3 | SFX_BD); // Everything normal
	PA_EnableSpecialFx(1,SFX_ALPHA,SFX_BG2 ,SFX_OBJ | SFX_BG3 | SFX_BD);
	PA_SetSFXAlpha(1,10,8);	
	PA_SetSFXAlpha(0,0,16);	
	RefreshDrawing(DSBmp.ScrollX, DSBmp.ScrollY,0);
	DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
	CheckGrid();
}
guispriteinfo CreateTextButton(s8 screen, u8 obj_num, u8 pal_num, s16 x, s16 y, char *text,s8 textX, s8 textY,u16 color){
	guispriteinfo TempSprite;
	TempSprite.Screen = screen;
	TempSprite.Sprite = obj_num;
	TempSprite.Pal = pal_num;
	TempSprite.X = x;
	TempSprite.Y = y;
	
	PA_LoadSpritePal(screen,pal_num,(void*)blankbutton_Pal);	
	PA_SetSpritePalCol(screen,pal_num,1,color);
	PA_CreateSprite(screen,obj_num,(void*)blankbutton_Sprite,OBJ_SIZE_32X16,1,pal_num,x,y);
	PA_InitAllSpriteDraw(); 
	PA_SpriteText(screen,obj_num,textX,textY,32,16,text,1,5,1,strlen(text));
	
	return TempSprite;
}
//Media Bar===============================================================================================================
//Media Bar===============================================================================================================
//Media Bar===============================================================================================================
#ifdef ENABLEOGG
int GetCharactersLength(char* text, const int font,s16 maxx){
	int i;
	u16 lx, letter;
	s16 len = 0;
	for (i = 0; text[ i ]; i++) {
		letter = text[ i ];
		lx = pa_bittextdefaultsize[ font ][ letter ] + textinfo.letterspacing;
		len += lx;
		if(len>maxx)return i;
	}
	return i;
}
unsigned int getTextPixels( const char* text, const int font ){
	int i;
	u8 lx, letter;
	u16 len = 0;
	for (i = 0; text[ i ]; i++){
		letter = text[ i ];
		lx = pa_bittextdefaultsize[ font ][ letter ] + textinfo.letterspacing;
		len += lx;
	}
	return( len );
}
void CreateTextSprite(s8 screen,s8 TotalSprites,s8 width, s8 height,s8 width2,s8 height2,s8 palette,void* pal,u16 col1, u16 col2,s16 x,s16 y,...){

	va_list vl;
	va_start(vl,y);
	
  
	PA_LoadSpritePal(screen,palette,(void*)pal);	// Palette name

	int OriginalBright=0;
	s8 red=0, green=0, blue=0;

	//main sprite
	red=col1&31;
	green=col1>>5&31;
	blue=col1>>10&31;
	OriginalBright=red+green+blue;
	PA_SetSpritePalCol(screen,palette,3,ColorBrightness(col1,OriginalBright-(93.0*0.25)));//dark boarder
	PA_SetSpritePalCol(screen,palette,2,col1);//main bg col
	PA_SetSpritePalCol(screen,palette,1,ColorBrightness(col1,OriginalBright+(93.0*0.40)));//light boarder
	//buttons
	red=col2&31;
	green=col2>>5&31;
	blue=col2>>10&31;
	OriginalBright=red+green+blue;
	PA_SetSpritePalCol(screen,palette,4,col2);//button dark
	PA_SetSpritePalCol(screen,palette,5,ColorBrightness(col2,OriginalBright+(93.0*0.40)));//button light
	
	int i=0,spritenum=va_arg(vl,int), xpos=x;
	for(i=0;i<TotalSprites;i++){
		PA_CreateSprite(screen, spritenum, (void*)mediabar_Sprite,width,height,1, palette,xpos, y);
		PA_SetSpriteAnim(screen, spritenum, i); 	
		spritenum=va_arg(vl,int);
		xpos+=width2;
	}
	va_end(vl);
	PA_InitAllSpriteDraw(); 

}

void WriteTextSprite(s8 screen,s16 x,s16 y,s8 width,s8 height,s8 palnum,u16 col,s8 font ,char *text,...){
	va_list vl;
	va_start(vl,text);
	
	int i=0,z=0,spritenum=va_arg(vl,int),textpos=0,textgap=0;
	s8 TotalSprites = (getTextPixels(text,font)/width)+1;
	
	PA_SetSpritePalCol(screen,PA_GetSpritePal(screen,spritenum),palnum,col);

	char NewText[128];
	memset(NewText,0,128);
	s32 maxwidth=100;
	
	strncpy (NewText,text,GetCharactersLength(text,font,maxwidth)-1);
	
	
	bool Short=0;
	for(i=0;i<TotalSprites;i++){
		Short=0;
		for(z=0;z<width;z++){//check if text is on non transparent
			if(PA_GetSpritePixel(screen,spritenum,z,y)==0){
				Short=1;
				textgap= GetCharactersLength(NewText+textpos,font,z);
				break;
			}	
		}	
		if(Short==0)textgap=GetCharactersLength(NewText+textpos,font,width);
		
		PA_SpriteText(screen,spritenum,x,y,width,height,NewText+textpos,palnum,font,1,textgap);
		spritenum=va_arg(vl,int);
		textpos+=textgap;
	}
	va_end(vl);
}
void CreateMediaBar(s8 screen,s32 x, s32 y){
	if(Settings.EnableAudio==1){//create media bar
		CreateTextSprite(0,2,OBJ_SIZE_64X32,64,32,2,(void*)mediabar_Pal,PA_RGB(24,26,28),PA_RGB(0,0,0),x,y,30,31);
		switch(OggStatus){
			default:break;
			case 1: WriteTextSprite(0,1,1,64,32,6,PA_RGB(15,0,0),5,"Stopped:",30,31);break;
			case 2:WriteTextSprite(0,1,1,64,32,6,PA_RGB(0,15,0),5,"Now Playing:",30,31);break;
			case 3:WriteTextSprite(0,1,1,64,32,6,PA_RGB(0,0,15),5,"Paused:",30,31);break;
		}	
		WriteTextSprite(0,1,9,64,32,5,PA_RGB(0,0,0),5,Ogg_CurrentSong,30,31);
		Box.MediaX=x;
		Box.MediaY=y;
	}
}
void UpdateMediaBar(s8 screen,s8 TotalSprites,...){

	if(Settings.EnableAudio){
		s32 x= Box.MediaX;
		s32 y= Box.MediaY;
		if(Stylus.Newpress){
			if(PA_StylusInZone(x+3,y+20,x+20,y+28)){//skip back
				Stop_OGG();
				Ogg_AutoPlay=0;
				if(Ogg_Track>0)Ogg_Track--;
				PlayOgg(Settings.audiopath,Ogg_Track);
				CreateMediaBar(0,Box.MediaX,Box.MediaY);
				Ogg_AutoPlay=1;
			}
			if(PA_StylusInZone(x+22,y+20,x+39,y+28)){//play 
				if(OggStatus==3)Pause_OGG();
				else{
					Stop_OGG();
					PlayOgg(Settings.audiopath,Ogg_Track);
					Ogg_AutoPlay=1;
				}
				CreateMediaBar(0,Box.MediaX,Box.MediaY);
			}		
			if(PA_StylusInZone(x+41,y+20,x+58,y+28)){//skip forward
				Stop_OGG();
				Ogg_AutoPlay=0;
				if(Ogg_Track<=Ogg_MaxTracks)Ogg_Track++;
				PlayOgg(Settings.audiopath,Ogg_Track);
				CreateMediaBar(0,Box.MediaX,Box.MediaY);
				Ogg_AutoPlay=1;
			}
			if(PA_StylusInZone(x+60,y+20,x+77,y+28)){//pause
				Pause_OGG();
				CreateMediaBar(0,Box.MediaX,Box.MediaY);
				Ogg_AutoPlay=0;
			}
			if(PA_StylusInZone(x+79,y+20,x+96,y+28)){//stop
				Stop_OGG();
				CreateMediaBar(0,Box.MediaX,Box.MediaY);
				Ogg_AutoPlay=0;
			}
			if(PA_StylusInZone(x,y,x+128,y+19)){
				int i=0, spritenum=0, differenceX=0,differenceY=0;
				va_list vl;
				va_start(vl,TotalSprites);
				spritenum=va_arg(vl,int);	
				
				differenceX= Stylus.X-x;
				differenceY= Stylus.Y-y;
				va_end(vl);
				while(Stylus.Held){
					va_list vl;
					va_start(vl,TotalSprites);
					spritenum=va_arg(vl,int);
					for(i=0;i<TotalSprites;i++){
						if(spritenum==30){
							Box.MediaX=Stylus.X-differenceX;
							Box.MediaY=Stylus.Y-differenceY;
						}
						PA_SetSpriteXY(screen,spritenum,Stylus.X+(64*i)-differenceX,Stylus.Y-differenceY);
						spritenum=va_arg(vl,int);
					}
					va_end(vl);
					Box.MediaX=Stylus.X-differenceX;
					Box.MediaY=Stylus.Y-differenceY;
					PA_WaitForVBL();
				}
			}
		}
	}
}	
#endif		
//Tool Bar================================================================================================================
//Tool Bar================================================================================================================
//Tool Bar================================================================================================================
void RunToolBarInterFace(void){
	LoadTB(Box.X-16,Box.Y);
	#ifdef ENABLEOGG
		CreateMediaBar(0,Box.MediaX,Box.MediaY);
	#endif
	FadeInInterface(0);
	Box.Created=1;
	while(Box.Created){
		UpdateStatsBar();
		UpdateZoom(Box.ToolSelected);
		#ifdef ENABLEOGG
			UpdateMediaBar(0,2,30,31);
		#endif
		ToolBoxControls();
		PA_SetBgRot(0, 3,DSBmp.HWScrollX,DSBmp.HWScrollY,0,0,0, Draw.Zoom);
	}
	FadeOutInterface(0);
	ResetTB();
	
	ClearInterFace();
	PA_EnableSpecialFx(0,SFX_ALPHA,SFX_OBJ | SFX_BG2 | SFX_BG1 ,SFX_BG0 | SFX_BG3 | SFX_BD); // Everything normal
	PA_SetSFXAlpha(0,0,16);	
}


//Main Interface================================================================================================================
//Main Interface================================================================================================================
//Main Interface================================================================================================================
void CreateColorSquares(bool Screen,s8 pal1,s8 pal2, s8 sprite1,s8 sprite2, s32 x1,s32 y1,s32 x2, s32 y2){
	ColorSquares[0].Screen=Screen;
	ColorSquares[1].Screen=Screen;
	ColorSquares[0].Sprite=sprite1;
	ColorSquares[1].Sprite=sprite2;
	ColorSquares[0].X=x1;ColorSquares[1].X=x2;
	ColorSquares[0].Y=y1;ColorSquares[1].Y=y2;
	
	PA_LoadSpritePal(ColorSquares[0].Screen,pal1,(void*)colorsquare_Pal);	// Palette name
	PA_LoadSpritePal(ColorSquares[1].Screen,pal2,(void*)colorsquare_Pal);	// Palette name	
	
	//sprite 1
	PA_CreateSprite(ColorSquares[0].Screen,ColorSquares[0].Sprite,(void*)colorsquare_Sprite,OBJ_SIZE_16X16,1,pal1,ColorSquares[0].X,ColorSquares[0].Y);
	PA_SetSpritePalCol(ColorSquares[0].Screen, ColorSquares[0].Sprite,1,Draw.Color[0]);
	PA_SetSpriteMode(ColorSquares[0].Screen,ColorSquares[0].Sprite,0);//set sprite mode to normal, fully transparent
	//sprite 2
	PA_CreateSprite(ColorSquares[1].Screen,ColorSquares[1].Sprite,(void*)colorsquare_Sprite,OBJ_SIZE_16X16,1,pal2,ColorSquares[1].X,ColorSquares[1].Y);
	PA_SetSpritePalCol(ColorSquares[1].Screen, ColorSquares[1].Sprite,1,Draw.Color[1]);
	PA_SetSpriteMode(ColorSquares[1].Screen,ColorSquares[1].Sprite,0);//set sprite mode to alpha blending, unused color
}
void UpdateColorSquares(bool swap){
	if(swap==0){
		PA_SetSpriteXY(ColorSquares[0].Screen,ColorSquares[0].Sprite,ColorSquares[0].X,ColorSquares[0].Y);
		PA_SetSpriteXY(ColorSquares[1].Screen,ColorSquares[1].Sprite,ColorSquares[1].X,ColorSquares[1].Y);
	}
	else{
		PA_SetSpriteXY(ColorSquares[0].Screen,ColorSquares[0].Sprite,ColorSquares[1].X,ColorSquares[1].Y);
		PA_SetSpriteXY(ColorSquares[1].Screen,ColorSquares[1].Sprite,ColorSquares[0].X,ColorSquares[0].Y);
	}
	if(DSBmp.Bits>8){
		PA_SetSpritePalCol(ColorSquares[0].Screen,ColorSquares[0].Sprite,1,Draw.Color[0]);
		PA_SetSpritePalCol(ColorSquares[1].Screen,ColorSquares[1].Sprite,1,Draw.Color[1]);
	}
	else{
		PA_SetSpritePalCol(ColorSquares[0].Screen,ColorSquares[0].Sprite,1,palette[Draw.Color[0]]);
		PA_SetSpritePalCol(ColorSquares[1].Screen,ColorSquares[1].Sprite,1,palette[Draw.Color[1]]);	
	}
}	

void LoadStatsBar(void){
	PA_ResetSpriteSysScreen(1);
	//PA_EnableSpecialFx	(1,1, 0,SFX_BG0 | SFX_BG1 | SFX_BG2 | SFX_BG3 | SFX_BD);
	CreateColorSquares(1,ColorSquarePals,0,1,213,174,233,174);
}	
void UpdateStatsBar(void){
	//stylus x and y position
	sprintf(InfoText,"X:%d \nY:%d      ",Stylus.X+DSBmp.ScrollX+DSBmp.HWScrollX, Stylus.Y+DSBmp.ScrollY+DSBmp.HWScrollY);
	PA_16bitText(1, 8,176, 255, 200, InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);	
	//layer and undo info
	sprintf(InfoText,"Layer:\n %d/%d ",DSBmp.LayerSelected,DSBmp.NumOfLayers);
	PA_16bitText(1, 116,176, 255,200, InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);	
	//debug
	sprintf(InfoText,"size:\n%d   ",PA_drawsize[0]);
	PA_16bitText(1, 50,176, 255,200, InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);	

	sprintf(InfoText,"zoom:\n %d",Draw.Zoom);
	PA_16bitText(1, 180,176, 255,200, InfoText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);	
	
	UpdateColorSquares(Box.SwapPal);
}
//misc interface
void BitModeCheckBox(s8 bitmode){
	switch(bitmode){
		case 8:
			PA_SetSpriteAnimEx(0,0,16,16,1,0);//24 bit unchecked
			PA_SetSpriteAnimEx(0,1,16,16,1,0);//16 bit unchecked
			PA_SetSpriteAnimEx(0,2,16,16,1,1);//8 bit checked
		break;
		case 16:
			PA_SetSpriteAnimEx(0,0,16,16,1,0);//24 bit unchecked
			PA_SetSpriteAnimEx(0,1,16,16,1,1);//16 bit checked
			PA_SetSpriteAnimEx(0,2,16,16,1,0);//8 bit unchecked
		break;
		case 24:
			PA_SetSpriteAnimEx(0,0,16,16,1,1);//24 bit checked
			PA_SetSpriteAnimEx(0,1,16,16,1,0);//16 bit unchecked
			PA_SetSpriteAnimEx(0,2,16,16,1,0);//8 bit unchecked
		break;
	}
}
