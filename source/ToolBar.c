#include <PA9.h>
#include "bgdata.h"

//submenus
s8 GenerateSubmenus(s8 tool){
	s8 i=0, count=0;
	switch(tool){
		case TB_Pencil:
			for(i=0;i<3;i++){
				Box.Tool[tool].SubIcons[i]=18+i;
				count++;
			}	
			return count;	
		break;
		case TB_Eraser:
			Box.Tool[tool].SubIcons[0]=18;count++;
			Box.Tool[tool].SubIcons[1]=20;count++;		
			return count+1;
		break;
		case TB_Shapes:
			for(i=0;i<6;i++){
				Box.Tool[tool].SubIcons[i]=11+i;
				count++;
			}
			return count;
		break;
		case TB_Marquis:
			for(i=0;i<3;i++){
				Box.Tool[tool].SubIcons[i]=16+i;
				count++;
			}	
			return count;
		break;
	}
	return 0;
}
//which tools have sliders
bool ToolSlider(u8 Tool){
	switch(Tool){
		case TB_Pencil:
			if(Box.ToolMode==1){
				Box.Tool[Tool].MaxVal=32;
				return 1;
			}
			if(Box.ToolMode==2){
				Box.Tool[Tool].MaxVal=12;
				return 1;
			}
		break;
		case TB_Shapes:
			if(Box.ToolSelected==TB_Shapes && (Box.ToolMode==2|| Box.ToolMode==4 || Box.ToolMode==5)){
				Box.Tool[Tool].MaxVal=32;
				return 1;
			}
		break;
		case TB_Eraser:
			if(Box.ToolMode >0){
				Box.Tool[Tool].MaxVal=32;
				return 1;
			}
		break;
		case TB_Zoom:
			Box.Tool[Tool].MaxVal=256;
			return 1;
		break;
		case TB_Grid:
			Box.Tool[Tool].MaxVal=6;
			return 1;
		break;
	}
	return 0;
}
void SetTBXY(s16 x,s16 y){
	u8 i=0;
	bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
	if(GET_FLAG(Box.Flags,TB_State_EpandMain)){//if main is expanded
		Box.X=x;Box.Y=y;
		if(!GET_FLAG(Box.Flags,TB_State_Hide)){
			PA_SetSpriteXY(Screen,Box.StartSprite,Box.X-4,Box.Y);//move the tab
			for(i=1;i<MainTools+1;i++){
				s32 Ypos= Box.Y+(16*(i-1));
				
				if(Ypos<192)PA_SetSpriteXY(Screen,Box.StartSprite+i,Box.X,Ypos);
				else PA_SetSpriteXY(Screen,Box.StartSprite+i,256,192);
			}
		}
	}
	else if(GET_FLAG(Box.Flags,TB_State_ExpandSub)){
		Box.X=x;Box.Y=y;
		if(!GET_FLAG(Box.Flags,TB_State_Hide)){
			PA_SetSpriteXY(Screen,Box.StartSprite,Box.X-4,Box.Y);//move tab
			s16 Sprites=GenerateSubmenus(Box.ToolSelected);
			for(i=1;i<Sprites+1;i++){
				s32 Ypos= Box.Y+(16*(i-1));
				
				if(Ypos<192)PA_SetSpriteXY(Screen,Box.StartSprite+i,Box.X,Ypos);
				else PA_SetSpriteXY(Screen,Box.StartSprite+i,256,192);
			}
		}
	}
	else if(GET_FLAG(Box.Flags,TB_State_ExpandSlide)){
		Box.X=x;Box.Y=y;
		if(!GET_FLAG(Box.Flags,TB_State_Hide)){
			PA_SetSpriteXY(Screen,Box.StartSprite,Box.X-4,Box.Y);//move tab
			s16 Sprites=2;
			for(i=1;i<Sprites+1;i++){
				PA_SetSpriteXY(Screen,Box.StartSprite+i,Box.X,Box.Y+(16*(i-1)));
			}
			if(ToolSlider(Box.ToolSelected)){
				Sprites= 7;
				PA_SetSpriteXY(Screen,Box.StartSprite+Box.BarSprite,Box.X+4,Box.Y+40 + Box.Tool[Box.ToolSelected].SliderY);
				for(i=0;i<Sprites;i++){
					s32 Ypos= Box.Y+14+(18*(i));
					
					if(Ypos<192)PA_SetSpriteXY(Screen,Box.StartSprite+i+MainTools+1,Box.X,Ypos);
					else PA_SetSpriteXY(Screen,Box.StartSprite+i+MainTools+1,256,192);
				}
			}
		}
	}
}

void CreateMainBar(void){
	s16 TempY=Box.Y;
	bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
	PA_InitSpriteExtPrio(1);
	PA_LoadSpritePal(Screen,PALNUM,(void*)toolbox_Pal);
	PA_SetSpritePalCol(Screen,PALNUM,1,Box.Color);
	
	if(!GET_FLAG(Box.Flags,TB_State_Hide)){
		PA_CreateSprite(Screen,Box.StartSprite,(void*)movetab_Sprite, OBJ_SIZE_32X32,1, PALNUM, 256, TempY);
		PA_SetSpriteExtPrio (Screen,Box.StartSprite,4);
	}
	int i=0, SpriteNum=0,count=1;
	//create main sprites
	for(i=1; i <MainTools+1; i++){
		if(!GET_FLAG(Box.Flags,TB_State_Hide)){
			PA_CreateSprite(Screen,Box.StartSprite+i,(void*)toolbox_Sprite, OBJ_SIZE_16X16,1, PALNUM, 256, 192);
			PA_SetSpriteAnimEx (Screen,Box.StartSprite+i,16,16,1,i-1);
		}
		SpriteNum=i;
		count++;
	}
	//create slider sprites
	s16 tempnum=SpriteNum+1;
	s8 SliderFrames[7]={0,0,1,1,1,1,2};
	s8 frame=0;
	for(i=tempnum;i<tempnum+7;i++){
		if(!GET_FLAG(Box.Flags,TB_State_Hide)){
			PA_CreateSprite(Screen,Box.StartSprite+i,(void*)tbbar_Sprite,OBJ_SIZE_32X32,1, PALNUM, 256, 192);
			PA_SetSpriteExtPrio (Screen,Box.StartSprite+i,1);
		}
		PA_SetSpriteAnimEx (Screen,Box.StartSprite+i,32,32,1,SliderFrames[frame]);		
		frame++;
		SpriteNum++;
		count++;
	}
	//create the slider
	Box.BarSprite=count;
	if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_CreateSprite(Screen,Box.StartSprite+count,(void*)slider_Sprite, OBJ_SIZE_8X8,1, PALNUM,256,192);
	count++;
	
	Box.Sprites=count;
	PA_EnableSpecialFx(0,SFX_ALPHA,SFX_OBJ | SFX_BG2 | SFX_BG1 ,SFX_BG0 | SFX_BG3 | SFX_BD); 
}

	
void DeleteToolBar(void){
	SET_FLAG(Box.Flags,TB_State_Hide);
	RESET_FLAG(Box.Flags,TB_State_Created);
	if(GET_FLAG(Box.Flags,TB_State_EpandMain))SET_FLAG(Box.Flags,TB_State_CollapseMain);
	if(GET_FLAG(Box.Flags,TB_State_ExpandSub))SET_FLAG(Box.Flags,TB_State_CollapseSub);
	if(GET_FLAG(Box.Flags,TB_State_ExpandSlide))SET_FLAG(Box.Flags,TB_State_CollapseSlide);
}
void FastDelTB(void){
	bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
	int i=0;
	for(i=Box.StartSprite;i<Box.StartSprite+ Box.Sprites;i++){
		PA_SetSpriteXY(Screen,i,256,192);
		PA_SetSpriteMode	(Screen,i,0);
	}
	DeleteToolBar();
}
		
void CreateToolBar(void){
	RESET_FLAG(Box.Flags,TB_State_Hide);
	SET_FLAG(Box.Flags,TB_State_Created);
	CreateMainBar();
	JumpToTool(Box.ToolSelected,Box.ToolMode);
	SetTBXY(Box.X,Box.Y);
}

void InitToolBar(s8 screen, s16 x, s16 y, u16 color,u8 StartSprite){
	memset(&Box,0,sizeof(Box));
	Box.StartSprite=StartSprite;
	if(screen)SET_FLAG(Box.Flags,TB_State_Screen);
	bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
	Box.Color=color;
	Box.X=x+16;Box.Y=y;
	u8 i=0;
	for(i=0;i<MainTools+1;i++){
		Box.Tool[i].SliderY=0;
		if(i!=TB_Zoom)Box.Tool[i].SliderValue=1;
		else Box.Tool[i].SliderValue=256;
	}	
	PA_SetSpritePalCol(Screen,0,1,Box.Color);
	SET_FLAG(Box.Flags,TB_State_MainExpanded);
	SET_FLAG(Box.Flags,TB_State_SubCollapsed);
	SET_FLAG(Box.Flags,TB_State_SlideCollapsed);//sliders are collapsed	
	SET_FLAG(Box.Flags,TB_State_Hide);
}

inline void ExpandMainBar(void){
	if(GET_FLAG(Box.Flags,TB_State_MainExpanded)==1 && !GET_FLAG(Box.Flags,TB_State_CollapseMain)){
		bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
		if(GET_FLAG(Box.Flags,TB_State_EpandMain)==0){	
			//collapse sub and slider
			if(!GET_FLAG(Box.Flags,TB_State_SubCollapsed))SET_FLAG(Box.Flags,TB_State_CollapseSub);
			if(!GET_FLAG(Box.Flags,TB_State_SlideCollapsed))SET_FLAG(Box.Flags,TB_State_CollapseSlide);
			if(GET_FLAG(Box.Flags,TB_State_SubCollapsed) && GET_FLAG(Box.Flags,TB_State_SlideCollapsed)){//once main is collapsed and slider, expand sub menu
				Box.Length=16;
				SET_FLAG(Box.Flags,TB_State_EpandMain);
				Box.ExpandTimer=1;
			}
		}
		else{
			if(!GET_FLAG(Box.Flags,TB_State_Hide)){
				PA_SetSpriteXY(Screen,Box.StartSprite,Box.X-4,Box.Y);
				PA_SetSpriteXY(Screen,Box.StartSprite+1,Box.X,Box.Y);
			}
			s16 j=Box.Y;
			if(Box.ExpandTimer<MainTools+1){
				if(!GET_FLAG(Box.Flags,TB_State_Hide)){
					if(j+(16*Box.ExpandTimer)-16<192)PA_SetSpriteXY(Screen,Box.StartSprite+Box.ExpandTimer,Box.X,j+(16*Box.ExpandTimer)-16);
				}
				PA_SetSpriteAnimEx (Screen,Box.StartSprite+Box.ExpandTimer,16,16,1,Box.ExpandTimer-1);
				Box.ExpandTimer++;
			}
			else{
				RESET_FLAG(Box.Flags,TB_State_MainExpanded);
				RESET_FLAG(Box.Flags,TB_State_MainCollapsed);
				Box.Length=(MainTools*16);
				Box.ToolSelected=0;
				Box.ToolMode=0;
			}
		}	
	}
}
inline void CollapseMainBar(void){
	if(GET_FLAG(Box.Flags,TB_State_CollapseMain) == 1){
		bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
		if(GET_FLAG(Box.Flags,TB_State_EpandMain)==1){	
			RESET_FLAG(Box.Flags,TB_State_EpandMain);
			Box.ExpandTimer=MainTools;
		}
		else{
			if(!GET_FLAG(Box.Flags,TB_State_Hide)){
				PA_SetSpriteXY(Screen,Box.StartSprite,Box.X-4,Box.Y);
				PA_SetSpriteXY(Screen,Box.StartSprite+1,Box.X,Box.Y);
			}
			if(Box.ExpandTimer>1){
				if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteXY(Screen,Box.StartSprite+Box.ExpandTimer,256,192);
				Box.ExpandTimer--;
			}
			else{
				PA_SetSpriteAnimEx (Screen,Box.StartSprite+1,16,16,1,Box.ToolSelected-1);
				RESET_FLAG(Box.Flags,TB_State_CollapseMain);
				SET_FLAG(Box.Flags,TB_State_MainCollapsed);
				Box.Length=16;
			}
		}
	}
}	

inline void ExpandSubMenu(s8 Tool){
	if(GET_FLAG(Box.Flags,TB_State_SubExpanded)==1 && !GET_FLAG(Box.Flags,TB_State_CollapseSub)){
		s16 Sprites=GenerateSubmenus(Tool);
		if(Sprites>0){
			bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
			if(GET_FLAG(Box.Flags,TB_State_ExpandSub)==0){
				//Collapse Main Menu if needed
				if(!GET_FLAG(Box.Flags,TB_State_MainCollapsed))SET_FLAG(Box.Flags,TB_State_CollapseMain);
				if(!GET_FLAG(Box.Flags,TB_State_SlideCollapsed))SET_FLAG(Box.Flags,TB_State_CollapseSlide);
				if(GET_FLAG(Box.Flags,TB_State_MainCollapsed) && GET_FLAG(Box.Flags,TB_State_SlideCollapsed)){//once main is collapsed and slider, expand sub menu
					Box.ExpandTimer=2;
					SET_FLAG(Box.Flags,TB_State_ExpandSub);
					Box.ToolMode=0;
				}
			}
			else {
				//set the top sprite to tool
				PA_SetSpriteAnimEx (Screen,Box.StartSprite+1,16,16,1,Tool-1);
				if(!GET_FLAG(Box.Flags,TB_State_Hide)){
					PA_SetSpriteXY(Screen,Box.StartSprite+1,Box.X,Box.Y);
					PA_SetSpriteXY(Screen,Box.StartSprite,Box.X-4,Box.Y);
				}
				s16 j=Box.Y;
				if(Box.ExpandTimer<Sprites+1){
					PA_SetSpriteAnimEx (Screen,Box.StartSprite+Box.ExpandTimer,16,16,1,Box.Tool[Tool].SubIcons[Box.ExpandTimer-2]);
					PA_SetSpriteAnimEx (Screen,Box.StartSprite+Box.ExpandTimer,16,16,1,Box.Tool[Tool].SubIcons[Box.ExpandTimer-2]);
					if(!GET_FLAG(Box.Flags,TB_State_Hide)){
						if(j+(16*Box.ExpandTimer)-16<192)PA_SetSpriteXY(Screen,Box.StartSprite+Box.ExpandTimer,Box.X,j+(16*Box.ExpandTimer)-16);
					}
					Box.ExpandTimer++;
				}
				else{
					RESET_FLAG(Box.Flags,TB_State_SubExpanded);
					RESET_FLAG(Box.Flags,TB_State_SubCollapsed);
					Box.Length=(Sprites*16);
					Box.ToolMode=0;
				}
			}
		}
		else RESET_FLAG(Box.Flags,TB_State_SubExpanded);
	}
}
inline void CollapseSub(s8 ToolMode){
	if(GET_FLAG(Box.Flags,TB_State_CollapseSub) == 1){
		s16 Sprites=GenerateSubmenus(Box.ToolSelected);
		if(Sprites>0){
			bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
			if(GET_FLAG(Box.Flags,TB_State_ExpandSub)==1){	
				RESET_FLAG(Box.Flags,TB_State_ExpandSub);
				Box.ExpandTimer=Sprites+1;
			}		
			else{
				//set the top sprite to the tool mode sprite
				PA_SetSpriteAnimEx (Screen,Box.StartSprite+1,16,16,1,Box.ToolSelected-1);
				if(!GET_FLAG(Box.Flags,TB_State_Hide)){
					PA_SetSpriteXY(Screen,Box.StartSprite+1,Box.X,Box.Y);
					PA_SetSpriteXY(Screen,Box.StartSprite,Box.X-4,Box.Y);
				}
				if(Box.ExpandTimer>1){
					if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteXY(Screen,Box.StartSprite+Box.ExpandTimer,256,Box.Y);
					Box.ExpandTimer--;
				}
				else{
					RESET_FLAG(Box.Flags,TB_State_CollapseSub);
					SET_FLAG(Box.Flags,TB_State_SubCollapsed);
					Box.Length=16;
				}
			}	
		}
		else RESET_FLAG(Box.Flags,TB_State_CollapseSub);
	}
}
inline void ExpandSlider(s8 Tool){
	if(GET_FLAG(Box.Flags,TB_State_SlideExpanded)==1 && !GET_FLAG(Box.Flags,TB_State_CollapseSlide)){
		bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
		if(GET_FLAG(Box.Flags,TB_State_ExpandSlide)==0){
			//if Main isn't collapsed then collapse it
			if(!GET_FLAG(Box.Flags,TB_State_MainCollapsed))SET_FLAG(Box.Flags,TB_State_CollapseMain);
			//if sub isn't collapsed, collapse it
			if(!GET_FLAG(Box.Flags,TB_State_SubCollapsed))SET_FLAG(Box.Flags,TB_State_CollapseSub);	
	
			if(GET_FLAG(Box.Flags,TB_State_MainCollapsed) && GET_FLAG(Box.Flags,TB_State_SubCollapsed)){//once main is collapsed and slider, expand sub menu
				Box.ExpandTimer=1;
				SET_FLAG(Box.Flags,TB_State_ExpandSlide);
			}	
		}
		else{	
			if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteXY(Screen,Box.StartSprite,Box.X-4,Box.Y);
			//set top sprite frame to tool
			PA_SetSpriteAnimEx (Screen,Box.StartSprite+1,16,16,1,Tool-1);
			if(Box.ToolMode>0){
				GenerateSubmenus(Box.ToolSelected);
				PA_SetSpriteAnimEx (Screen,Box.StartSprite+1,16,16,1,Box.Tool[Box.ToolSelected].SubIcons[Box.ToolMode-1]);
			}
			if(!GET_FLAG(Box.Flags,TB_State_Hide)){
				PA_SetSpriteXY(Screen,Box.StartSprite+1,Box.X,Box.Y);
				//set the second sprite to the pal swap sprite
				PA_SetSpriteXY(Screen,Box.StartSprite+2,Box.X,Box.Y+16);
			}
			PA_SetSpriteAnimEx (Screen,Box.StartSprite+2,16,16,1,10);
			if(ToolSlider(Tool)){
				//s8 SliderFrames[7]={0,1,1,1,1,2};
				Box.Length=32;	
				s16 j=Box.Y+14;			
				if(Box.ExpandTimer<7){
					if(!GET_FLAG(Box.Flags,TB_State_Hide)){
						if(j + (18*Box.ExpandTimer)<192)PA_SetSpriteXY(Screen,Box.StartSprite+MainTools+1+Box.ExpandTimer,Box.X,j + (18*Box.ExpandTimer));
					}
					Box.ExpandTimer++;
				}
			}
			else Box.ExpandTimer=7; 
			if(Box.ExpandTimer>=7){
				//set the slider bar
				if(ToolSlider(Tool)){
					if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteXY(Screen,Box.StartSprite+Box.BarSprite,Box.X+4,Box.Y+40 + Box.Tool[Box.ToolSelected].SliderY);
				}
				RESET_FLAG(Box.Flags,TB_State_SlideExpanded);
				RESET_FLAG(Box.Flags,TB_State_SlideCollapsed);
				if(ToolSlider(Tool))Box.Length=32+(6*18);	
				else Box.Length=32;
			}
			
		}
	}
}
inline void CollapseSlider(s8 Tool){	
	if(GET_FLAG(Box.Flags,TB_State_CollapseSlide) == 1){
		bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
		if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteXY(Screen,Box.StartSprite+Box.BarSprite,256,192);
		if(GET_FLAG(Box.Flags,TB_State_ExpandSlide)==1){	
			RESET_FLAG(Box.Flags,TB_State_ExpandSlide);
			Box.ExpandTimer=2;
			if(ToolSlider(Tool) || GET_FLAG(Box.Flags,TB_State_ForceSlideCollapse))Box.ExpandTimer=MainTools+7;
			RESET_FLAG(Box.Flags,TB_State_ForceSlideCollapse);
		}
		else{
			if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteXY(Screen,Box.StartSprite,Box.X-4,Box.Y);
			//set top sprite frame to tool
			PA_SetSpriteAnimEx (Screen,Box.StartSprite+1,16,16,1,Tool-1);
			if(Box.ToolMode>0){
				GenerateSubmenus(Box.ToolSelected);
				PA_SetSpriteAnimEx (Screen,Box.StartSprite+1,16,16,1,Box.Tool[Box.ToolSelected].SubIcons[Box.ToolMode-1]);
			}
			if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteXY(Screen,Box.StartSprite+1,Box.X,Box.Y);
			if(Box.ExpandTimer>MainTools){
				if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteXY(Screen,Box.StartSprite+Box.ExpandTimer,256,Box.Y);
				Box.ExpandTimer--;
			}
			if(Box.ExpandTimer==MainTools)Box.ExpandTimer=2;
			if(Box.ExpandTimer<=MainTools && Box.ExpandTimer>1){
				if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteXY(Screen,Box.StartSprite+Box.ExpandTimer,256,Box.Y);
				Box.ExpandTimer--;
			}
			if(Box.ExpandTimer==1){
				RESET_FLAG(Box.Flags,TB_State_CollapseSlide);
				SET_FLAG(Box.Flags,TB_State_SlideCollapsed);
				Box.Length=16;
				//Box.ToolMode=0;
			}
		}	
	}
}	
void ToolBarCheckExpands(void){
	bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
	bool Continue=1;
	//check top sprite to see which menus to expand
	if(PA_SpriteTouched(Box.StartSprite+1)){
		//SET_FLAG(Box.Flags,TB_State_StylusPress);
		RESET_FLAG(Box.Flags,TB_State_Move);
		s8 i=0;
		//check if in slider mode 
		s16 Sprites=GenerateSubmenus(Box.ToolSelected);
		if(Sprites>0){
			for(i=0;i<Sprites+1;i++){
				if(PA_GetSpriteAnimFrame (Screen,Box.StartSprite+1)  == Box.Tool[Box.ToolSelected].SubIcons[i]){
					SET_FLAG(Box.Flags,TB_State_SubExpanded);//expand sub
					Continue=0;
				}
			}
		}
		//check if in sub menu
		if(Continue){
			for(i=1;i<MainTools+1;i++){
				if(PA_GetSpriteAnimFrame (Screen,Box.StartSprite+1)  == (i-1)){
					SET_FLAG(Box.Flags,TB_State_MainExpanded);//expand sub
					Continue=0;
				}
			}
		}
	}
}
void ToolBarCheckMain(void){
	if(GET_FLAG(Box.Flags,TB_State_EpandMain)){
		s8 i=0;
		for(i=1;i<MainTools+1;i++){
			if(PA_SpriteTouched(Box.StartSprite+i)){
				//SET_FLAG(Box.Flags,TB_State_StylusPress);
				if(i!=1){
					RESET_FLAG(Box.Flags,TB_State_Move);
					Box.ToolSelected=i;
					s16 Sprites=GenerateSubmenus(Box.ToolSelected);
					if(Sprites>0)SET_FLAG(Box.Flags,TB_State_SubExpanded);
					else SET_FLAG(Box.Flags,TB_State_SlideExpanded);// slider doesn't expand for all tools, just ones that need it
				}
				else SET_FLAG(Box.Flags,TB_State_Move);
			}
		}
	}
}
void ToolBarCheckSub(void){	
	if(GET_FLAG(Box.Flags,TB_State_ExpandSub)){
		s8 i=0;
		s16 Sprites=GenerateSubmenus(Box.ToolSelected);
		for(i=2;i<Sprites+1;i++){
			if(PA_SpriteTouched(Box.StartSprite+i)){
				//SET_FLAG(Box.Flags,TB_State_StylusPress);
				RESET_FLAG(Box.Flags,TB_State_Move);
				Box.ToolMode=i-1;
				SET_FLAG(Box.Flags,TB_State_SlideExpanded);// slider doesn't expand for all tools, just ones that need it
			}
		}
	}	
}	
void ToolBarSwapPal(void){
	if(GET_FLAG(Box.Flags,TB_State_ExpandSlide)){
		SET_FLAG(Box.Flags,TB_State_StylusPress);
		//check PalSwap
		if(PA_SpriteTouched(Box.StartSprite+2)){
			Box.SwapPal++;
			if(Box.SwapPal>1)Box.SwapPal=0;
		}
	}
}
void ToolBarCheckSlide(void){
	if(GET_FLAG(Box.Flags,TB_State_ExpandSlide)){
		bool Screen = GET_FLAG(Box.Flags,TB_State_Screen);
		//check slider
		if(PA_StylusInZone(Box.X,Box.Y+42,Box.X+16,Box.Y+40+92)){
			Box.Tool[Box.ToolSelected].SliderY=(Stylus.Y-4)-(Box.Y+40);
			if(!GET_FLAG(Box.Flags,TB_State_Hide))PA_SetSpriteY(Screen,Box.StartSprite+Box.BarSprite,Stylus.Y-4);
			
			if(Box.ToolSelected!=TB_Zoom)Box.Tool[Box.ToolSelected].SliderValue = (Box.Tool[Box.ToolSelected].SliderY *Box.Tool[Box.ToolSelected].MaxVal)/92;
			else Box.Tool[Box.ToolSelected].SliderValue =SCREEN_WIDTH - (Box.Tool[Box.ToolSelected].SliderY *Box.Tool[Box.ToolSelected].MaxVal)/92;
			
			
			if(Box.Tool[Box.ToolSelected].SliderValue>Box.Tool[Box.ToolSelected].MaxVal)Box.Tool[Box.ToolSelected].SliderValue=Box.Tool[Box.ToolSelected].MaxVal;
			if(Box.Tool[Box.ToolSelected].SliderValue<0)Box.Tool[Box.ToolSelected].SliderValue=0;	
		}
	}
}		
void ToolBarInuput(void){
	if(Stylus.Newpress){
		if(!GET_FLAG(Box.Flags,TB_State_Move)){
			ToolBarCheckExpands();
			ToolBarCheckMain();
			ToolBarCheckSub();
			ToolBarSwapPal();
		}
		if(PA_SpriteTouched(Box.StartSprite) && !PA_SpriteTouched(Box.StartSprite+1) && PA_StylusInZone(Box.X-4,Box.Y, Box.X,Box.Y+19))	SET_FLAG(Box.Flags,TB_State_Move);
	}
	if(Stylus.Held){
		//if(!GET_FLAG(Box.Flags,TB_State_StylusPress))
		ToolBarCheckSlide();
		if(GET_FLAG(Box.Flags,TB_State_Move))SetTBXY(Stylus.X-8,Stylus.Y-8);
	}
	else if(!Stylus.Newpress || !Stylus.Held){
		//RESET_FLAG(Box.Flags,TB_State_StylusPress);
		RESET_FLAG(Box.Flags,TB_State_Move);
	}
}

void JumpToTool(s8 Tool,s8 subtool){
	Box.OldTool=Box.ToolSelected; Box.OldMode = Box.ToolMode;
	
	Box.ToolSelected=Tool;
	Box.ToolMode=subtool;
	s16 Sprites=GenerateSubmenus(Tool);
	if(Tool==0){
		if(!GET_FLAG(Box.Flags,TB_State_MainCollapsed))SET_FLAG(Box.Flags,TB_State_MainCollapsed);
		SET_FLAG(Box.Flags,TB_State_MainExpanded);
	}
	else{
		if(subtool==0 && Sprites>0){
			if(!GET_FLAG(Box.Flags,TB_State_SubCollapsed))SET_FLAG(Box.Flags,TB_State_CollapseSub);
			SET_FLAG(Box.Flags,TB_State_SubExpanded);//expand sub menu
		}
		if(subtool>0 && GET_FLAG(Box.Flags,TB_State_SlideCollapsed))SET_FLAG(Box.Flags,TB_State_SlideExpanded);
		
		
		if(ToolSlider(Tool) && subtool==0){//expand slider
			if(!GET_FLAG(Box.Flags,TB_State_SlideCollapsed))SET_FLAG(Box.Flags,TB_State_CollapseSlide);
			if(!GET_FLAG(Box.Flags,TB_State_SubCollapsed) && !GET_FLAG(Box.Flags,TB_State_UpdateJump)){
				Box.ToolSelected=Box.OldTool;
				Box.ToolMode= Box.OldMode;
				Box.DestTool=Tool; Box.DestMode = subtool;
				SET_FLAG(Box.Flags,TB_State_CollapseSub);
				SET_FLAG(Box.Flags,TB_State_UpdateJump); 
			}
			SET_FLAG(Box.Flags,TB_State_SlideExpanded);	
		}
		
		if(Sprites>0 && subtool>0){//if a tool in submenu is selected
			SET_FLAG(Box.Flags,TB_State_ForceSlideCollapse);
			SET_FLAG(Box.Flags,TB_State_CollapseSlide);
			SET_FLAG(Box.Flags,TB_State_SlideExpanded);
		}
		
		if(Sprites==0 && subtool==0){//not in submenu but in a main tool with no slider
			if(!GET_FLAG(Box.Flags,TB_State_SlideCollapsed)){
				SET_FLAG(Box.Flags,TB_State_CollapseSlide);
				SET_FLAG(Box.Flags,TB_State_ForceSlideCollapse);
			}
			if(!GET_FLAG(Box.Flags,TB_State_SubCollapsed) && !GET_FLAG(Box.Flags,TB_State_UpdateJump)){
				Box.ToolSelected=Box.OldTool;
				Box.ToolMode= Box.OldMode;
				Box.DestTool=Tool; Box.DestMode = subtool;
				SET_FLAG(Box.Flags,TB_State_CollapseSub);
				SET_FLAG(Box.Flags,TB_State_UpdateJump); 
			}
			SET_FLAG(Box.Flags,TB_State_SlideExpanded);
		}
	}
}


inline void UpdateToolBar(void){	
	if(GET_FLAG(Box.Flags,TB_State_Created))ToolBarInuput();
	ExpandMainBar();
	CollapseMainBar();
	ExpandSubMenu(Box.ToolSelected);
	CollapseSub(Box.ToolMode);
	if(GET_FLAG(Box.Flags,TB_State_SubCollapsed)&& GET_FLAG(Box.Flags,TB_State_UpdateJump)){
		Box.ToolSelected=Box.DestTool;
		Box.ToolMode= Box.DestMode;	
		RESET_FLAG(Box.Flags,TB_State_UpdateJump);
	}
		
	ExpandSlider(Box.ToolSelected);
	CollapseSlider(Box.ToolSelected);
}