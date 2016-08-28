#include <PA9.h>
#include "BAGKeyboard.h"
#include "bgdata.h"

void RedrawSaveScreen(void){
	PA_TextLetterSpacing(-5);
	PA_16bitText(0,0,2, 255,192,"Save as:", PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 6, 1, 100);
	PA_Draw16bitLineEx (0,235,0,235,14,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),1);
	PA_TextLetterSpacing(0);
	//save as buttons
	PA_16bitText(0,131,135, 255,192,"Save as\n  Bmp", PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 5, 1, 100);
	PA_16bitText(0,80,135, 255,192,"Save EPF\n Project", PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB), 5, 1, 100);
	PA_Draw16bitLineEx (0,74,130,74,153,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),1);
	PA_Draw16bitLineEx (0,122,130,122,153,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),1);
	PA_Draw16bitLineEx (0,169,130,169,153,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),1);
	PA_Draw16bitLineEx (0,74,153,169,153,PA_RGB(Settings.LrgTxtR,Settings.LrgTxtG,Settings.LrgTxtB),1);
	//draw X
	PA_Draw16bitLineEx (0,241,1,251,11,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue),1);
	PA_Draw16bitLineEx (0,241,11,251,1,PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue),1);	
	
	PA_Draw16bitLineEx (0,0,LargeTextHeight-3,256,LargeTextHeight-3,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),2);
	//SetKeyBoardScreen(0);
	//Draw8bitKeyboard(KeyBoards.type,29,49,16,16,8);
	
	//bit depth
	PA_Draw16bitLineEx (0,0,160,256,160,PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue),1);//top
	PA_16bitText(0,15,174, 256,192,"Bit Mode:", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	PA_16bitText(0,92,174, 256,192,"24 bit", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	PA_16bitText(0,144,174, 256,192,"16 bit", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);
	if(DSBmp.Bits==8)PA_16bitText(0,193,174, 256,192,"8 bit", PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 1, 100);	
}
void SaveMenu(void){
	DSBmp.PrgmLocation = Location_Save;
	ClearScreen(0);
	PA_SetBgRot(0, 3,0,0,0,0,0, 256);

	RedrawSaveScreen();
	
	SetKeyBoardScreen(0);
	SetKeyBoardBgMode(KB_BG_16bit);
	SetKeyBoardFont(2);
	SetKeyBoardColors(Settings.KeyboardCol[2],Settings.KeyboardCol[0],Settings.KeyboardCol[3],Settings.KeyboardCol[1]);
	BAG_InitKeyBoard(29,49,16,8);	
	
	s8 TempBits=DSBmp.Bits;
	//check boxes
	PA_LoadSpritePal(0,0,(void*)checkbox_Pal);
	PA_CreateSprite(0,0,(void*)checkbox_Sprite, OBJ_SIZE_16X16,1, 0,117,170);//24 bit
	PA_CreateSprite(0,1,(void*)checkbox_Sprite, OBJ_SIZE_16X16,1, 0,167,170);//16 bit
	if(DSBmp.Bits==8)PA_CreateSprite(0,2,(void*)checkbox_Sprite, OBJ_SIZE_16X16,1, 0,213,170);//8 bit
	BitModeCheckBox(TempBits);

	PA_SetBgPrio(0,3,1);
	PA_LoadBackground(0, 2, &bar);
	PA_SetBgPrio(0,2,3);
	PA_SetBgPalNCol (0,2,0,1, PA_RGB(Settings.HighlightRed,Settings.HighlightGreen,Settings.HighlightBlue));
	PA_BGScrollXY(0,2,-10,-25);

	char FileNameText[128];
	memset(&FileNameText,0,128);
	int i=0;
	char newletter=0;
	bool SaveMenu=1;
	char savetext[128];
	memset(&savetext,0,128);
	sprintf(FileNameText,"%s",LastFile);
	i=strlen(LastFile);
	PA_16bitText(0,10,25, 256,192, FileNameText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);
	
	while (SaveMenu==1){
		if(Stylus.Newpress){
			newletter=CheckKeyboard();
			if(newletter!=0 && newletter!=SHIFT && newletter!=BACKSPACE && newletter!=TAB && newletter!=CAPS && newletter!=NO_KEY){
				FileNameText[i]=newletter;
				i++;
			}
			else if(newletter==BACKSPACE){
				FileNameText[i]='\0';
				if(i>0)i--;
				FileNameText[i]='\0';
			}
		}
		if(Stylus.Released && newletter!=0){;
			RefreshKeyboard();
			Fill16bitBlock(10,25,10,256,0,PA_DrawBg[0]);
			PA_16bitText(0,10,25, 256,192, FileNameText, PA_RGB(Settings.TxtRed,Settings.TxtGreen,Settings.TxtBlue), 5, 0, 100);	
		}
		//exit
		if(Stylus.Newpress){
			if(PA_StylusInZone(235,0,256,14)){
				DeleteSaveScreen();
				SaveMenu=0;
			}
			//save etch project
			if(PA_StylusInZone(74,130,121,153) && i!=0){
				PA_DeleteBg(0,2);
				sprintf(LastFile,"%s",FileNameText);
				sprintf(savetext,"%s",FileNameText);
				SaveEPFFile(savetext);
				SaveMenu=0;
			}
			//save bmp
			if(PA_StylusInZone(122,130,169,153) && i!=0){	
				PA_DeleteBg(0,2);
				sprintf(LastFile,"%s",FileNameText);
				sprintf(savetext,"%s",FileNameText);
				screenshotbmp(savetext,TempBits);
				SaveMenu=0;
			}	
			
			//bit mode
			if(PA_SpriteTouched(0)){
				TempBits=24;
				BitModeCheckBox(TempBits);
			}
			if(PA_SpriteTouched(1)){
				TempBits=16;
				BitModeCheckBox(TempBits);
			}			
			if(DSBmp.Bits==8 && PA_SpriteTouched(2)){
				TempBits=8;
				BitModeCheckBox(TempBits);
			}			
		}
		
		PA_WaitForVBL();
	}
}
