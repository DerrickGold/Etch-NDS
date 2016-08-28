#include <PA9.h>
#include "BAGKeyboard.h"
#include "bgdata.h"
void RedrawKBScreen(void){
	PA_TextLetterSpacing(-5);
	PA_16bitText(1,0,2, 255,192,"Typing:", PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 6, 1, 100);
	PA_Draw16bitLineEx (1,235,0,235,15,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),1);
	PA_TextLetterSpacing(0);
	
	//draw X
	PA_Draw16bitLineEx (1,241,1,251,11,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue),1);
	PA_Draw16bitLineEx (1,241,11,251,1,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue),1);	
	
	PA_Draw16bitLineEx (1,0,LargeTextHeight-3,256,LargeTextHeight-3,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),2);
	//SetKeyBoardScreen(1);
	//Draw8bitKeyboard(KeyBoards.type,29,80,16,16,8);
}
s8 kb_DrawText(const char *string,int font_size,u16 color, bool Blit){
	switch(Blit){
		case 0:
			Font_SetScreenBuf(0,bglayer[LayerOrder[DSBmp.LayerSelected]]);
			return(BAG_OutputText(0,Draw.X1,Draw.Y1,Draw.X2,Draw.Y2,string,1,font_size,0,color));
		break;
		case 1:
			//RefreshDrawing(DSBmp.ScrollX, DSBmp.ScrollY,1);
			UndoTool();
			Font_SetScreenBuf(0,bg_array[0]);
			return(Font_OutputText(0,Draw.X1,Draw.Y1,Draw.X2,Draw.Y2,string,1,font_size,0,color));
		break;
	}
	return 0;
}
//typing on images
void CreateKBMenu(void){	
	ClearScreen(1);ClearScreen(0);
	//RefreshDrawing(DSBmp.ScrollX, DSBmp.ScrollY,0);
	UndoTool();
	DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
	RedrawKBScreen();
	
	SetKeyBoardScreen(1);
	SetKeyBoardBgMode(KB_BG_16bit);
	SetKeyBoardFont(1);
	SetKeyBoardColors(Settings.KeyboardCol[2],Settings.KeyboardCol[0],Settings.KeyboardCol[3],Settings.KeyboardCol[1]);
	BAG_InitKeyBoard(8,88,20,8);
	
	PA_SwitchScreens();
	PA_WaitForVBL();PA_WaitForVBL();

	//set up font
	Font_SetScreenBuf(0,bg_array[0]);//pointer to location
	int font_size = FT_FIXED(12);
	
	bool kbmenu=1;
	char TextToWrite[256];
	memset(TextToWrite,0,256);
	int i=0;
	char newletter=0;
	PA_SetBgRot(0, 3,DSBmp.HWScrollX,DSBmp.HWScrollY,0,0,0, Draw.Zoom);
	LineRect16c(0,Draw.BGX1,Draw.BGY1,Draw.BGX2,Draw.BGY2);
	while(kbmenu){
		//check keyboard stuff
		if(Stylus.Newpress){
			newletter=CheckKeyboard();
			if(newletter!=0 && newletter!=SHIFT && newletter!=BACKSPACE && newletter!=TAB && newletter!=CAPS){
				TextToWrite[i]=newletter;
				i++;
			}
			else if(newletter==BACKSPACE){
				if(i>0)i--;
				TextToWrite[i]=0;
			}
		}
		//update text when letter is pressed
		if(Stylus.Released && newletter!=0){
			//PA_Clear16bitBg(1);
			//RedrawKBScreen();
			RefreshKeyboard();
			i=kb_DrawText(TextToWrite,font_size,Draw.Color[Box.SwapPal], 1);
			DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
		}
		//Change font sizes on the fly
		if (Pad.Held.Up) {
			font_size += FT_FIXED(1) / 4;
			kb_DrawText(TextToWrite,font_size,Draw.Color[Box.SwapPal], 1);
			DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
		}
		if (Pad.Held.Down) {
			font_size -= FT_FIXED(1) / 4;
			kb_DrawText(TextToWrite,font_size,Draw.Color[Box.SwapPal], 1);
			DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
		}	
		//exit
		if(Stylus.Newpress){
			if(PA_StylusInZone(235,0,256,14)){
				CalculateScale(DSBmp.Width,DSBmp.Height,Settings.MiniviewWidth,Settings.MiniviewHeight);
				kbmenu=0;
			}
		}
		if(Pad.Newpress.Start){
			kb_DrawText(TextToWrite,font_size,Draw.Color[Box.SwapPal], 0);
			kbmenu=0;
		}
			
		PA_vblFunc();
		
	}
	PA_SwitchScreens();
	LoadMainInterFace();
	if(Box.Created==1)Box.Created=0;
}