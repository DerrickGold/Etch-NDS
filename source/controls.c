#include <PA9.h>
#include "bgdata.h"

s16 StylusOldX[2], StylusOldY[2];

void UpdateStylusOld(void)
{
	if(DSBmp.Width>=SCREEN_WIDTH)
		StylusOldX[0]=((Stylus.X*Draw.Zoom)>>8)+DSBmp.ScrollX;
	else 
	{
		StylusOldX[0]=((Stylus.X*Draw.Zoom)>>8)+DSBmp.HWScrollX;
		StylusOldX[1]=Stylus.X-DSBmp.CanvasX;
	}
	if(DSBmp.Height>=SCREEN_HEIGHT)
		StylusOldY[0]=((Stylus.Y*Draw.Zoom)>>8)+DSBmp.ScrollY;
	else 
	{
		StylusOldY[0]=((Stylus.Y*Draw.Zoom)>>8)+DSBmp.HWScrollY;
		StylusOldY[1]=Stylus.Y-DSBmp.CanvasY;
	}
}
void Panning(u8 *buttonHeld)
{
	bool UpdateScrollBG=0;
	LoadStylusCursor(0,3,1,TB_Move,(256>>1)-(16>>1),(192>>1)-(16>>1));
	while(*buttonHeld)
	{
		//screen limits
		PA_UpdatePad();
		PA_UpdateStylus();
		if(Stylus.Newpress)
		{
			UpdateStylusOld();
			SetCursorFrame(8);
		}
		while(Stylus.Held)
		{
			SetCursorXY(Stylus.X, Stylus.Y);
			RedrawScreen=1;
			DSBmp.UpdateView=0;
			if(DSBmp.Width>=SCREEN_WIDTH)
			{
				DSBmp.ScrollX+=(StylusOldX[0]-((Stylus.X*Draw.Zoom)>>8))-DSBmp.ScrollX;
				
				while(DSBmp.ScrollX <-((40*Draw.Zoom)>>8)) 
					DSBmp.ScrollX++;
				while(DSBmp.ScrollX > DSBmp.Width+((40*Draw.Zoom)>>8) - Draw.Zoom) 
					DSBmp.ScrollX--;
				DSBmp.CenterX=DSBmp.ScrollX+(Draw.Zoom>>1); 
			}
			else 
			{
				s32 CalcCanvasLowX=DSBmp.TempCanvasX-((DSBmp.TempCanvasX*Draw.Zoom)>>8);
				s32 CalcCanvasMaxX= DSBmp.Width+DSBmp.TempCanvasX-((DSBmp.TempCanvasX+DSBmp.Width)*Draw.Zoom>>8);
				
				DSBmp.HWScrollX+=(StylusOldX[0]-((Stylus.X*Draw.Zoom)>>8))-DSBmp.HWScrollX;
				
				while(DSBmp.HWScrollX>CalcCanvasMaxX)
					DSBmp.HWScrollX--;
				while(DSBmp.HWScrollX<CalcCanvasLowX)
					DSBmp.HWScrollX++;
					
				DSBmp.CenterX=DSBmp.HWScrollX+(Draw.Zoom>>1);
				UpdateScrollBG=1;
			}
			if(DSBmp.Height>=SCREEN_HEIGHT)
			{
				DSBmp.ScrollY +=(StylusOldY[0]-((Stylus.Y*Draw.Zoom)>>8))-DSBmp.ScrollY;
				
				while(DSBmp.ScrollY < -((40*Draw.Zoom)>>8)) 
					DSBmp.ScrollY++;
				while(DSBmp.ScrollY > DSBmp.Height+((40*Draw.Zoom)>>8) - ((Draw.Zoom*SCREEN_HEIGHT)>>8)) 
					DSBmp.ScrollY--;
					
				DSBmp.CenterY=DSBmp.ScrollY+(Draw.Zoom>>1); 
			}
			else 
			{
				s32 CalcCanvasLowY=DSBmp.TempCanvasY-((DSBmp.TempCanvasY*Draw.Zoom)>>8);
				s32 CalcCanvasMaxY= DSBmp.Height+DSBmp.TempCanvasY-((DSBmp.TempCanvasY+DSBmp.Height)*Draw.Zoom>>8);
				
				DSBmp.HWScrollY+=(StylusOldY[0]-((Stylus.Y*Draw.Zoom)>>8))-DSBmp.HWScrollY;
				
				while(DSBmp.HWScrollY>CalcCanvasMaxY)
					DSBmp.HWScrollY--;
				while(DSBmp.HWScrollY<CalcCanvasLowY) 
					DSBmp.HWScrollY++;
					
				DSBmp.CenterY=DSBmp.HWScrollY+(Draw.Zoom>>1);
				UpdateScrollBG=1;
			}
			
			RefreshDrawing(DSBmp.ScrollX,DSBmp.ScrollY,Settings.FastScroll);
			if(RedrawScreen==1)
			{
				DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
				DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
			}
			if(UpdateScrollBG)
			{
				PA_WaitForVBL();
				PA_SetBgRot(0, 3,DSBmp.HWScrollX,DSBmp.HWScrollY,0,0,0, Draw.Zoom);
				UpdateScrollBG=0;
			}
			UpdateStatsBar();
			ViewSquare(DSBmp.MiniViewX,DSBmp.MiniViewY,DSBmp.XScale,DSBmp.YScale,0);
		}
		if(Stylus.Released)
		{	
			UpdateStylusOld();
			if(Settings.FastScroll==1)
			{
				RefreshDrawing(DSBmp.ScrollX, DSBmp.ScrollY,0);
				DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
				DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);	
			}
			SetCursorFrame(7);
		}
	}
	RESET_FLAG(Display.Flag,SCROLL);
}
void StylusScrollBg(u8* buttonHeld)
{
	if(Box.Created==1)
	{
		ResetTB();
		Box.Created=0;
	}
	SET_FLAG(Display.Flag,SCROLL);
	Display.Button = (u8*)buttonHeld;
}
void PrepColInterface(u8* buttonHeld)
{
	if(Box.Created==1)
	{
		ResetTB();
		Box.Created=0;
	}
	SET_FLAG(Display.Flag,COLMEN);
	Display.Button = (u8*)buttonHeld;
}

void UseTool(void){
	if(DSBmp.toolused==0 && Box.ToolSelected!=0)
	{
		CheckSaveUndo();
		LoadStylusCursor(0,3,1,Box.ToolSelected,(256>>1)-(16>>1),(192>>1)-(16>>1));
		DSBmp.toolused=1;
	}
	s32 x=0, y=0;
	switch(Box.ToolSelected)
	{
		case TB_Pencil:
			PA_SetDrawSize(0,Box.Tool[Box.ToolSelected].SliderValue);
			RedrawScreen=1;
			switch(Box.ToolMode)
			{
				case 1://square pencil
					BAG_16bitDraw(0,Stylus.X,Stylus.Y,Draw.Color[Box.SwapPal],0);
				break;
				case 2://circle pencil
					BAG_16bitCircleDraw(0,Stylus.X,Stylus.Y,Draw.Color[Box.SwapPal]);
				break;
			}
			DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
		break;
		case TB_Eraser:
			PA_SetDrawSize(0,Box.Tool[Box.ToolSelected].SliderValue);
			switch(Box.ToolMode){
				case 1://non trans eraser
					if(DSBmp.Bits>8)
						BAG_16bitDraw(0,Stylus.X,Stylus.Y,PA_RGB(31,31,31),0);
					else 
						BAG_16bitDraw(0,Stylus.X,Stylus.Y,DSBmp.EraserPal,0);
				break;
				case 2://trans eraser
					if(DSBmp.LayerSelected!=0){
						if(DSBmp.Bits>8)
							BAG_16bitDraw(0,Stylus.X,Stylus.Y,Trans24bit,0);
						else 
							BAG_16bitDraw(0,Stylus.X,Stylus.Y,Trans8bit+1,0);		
					}
				break;
			}
			DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
		break;
		case TB_Shapes:
			switch(Box.ToolMode){
				case 1://filled rect
					PA_SetDrawSize(0,0);
					Rectangle(0,Draw.Color[Box.SwapPal]);
				break;
				case 2://line rect
					PA_SetDrawSize(0,Box.Tool[Box.ToolSelected].SliderValue);
					LineRectangle(0,Draw.Color[Box.SwapPal]);
				break;
				case 3://circle
					PA_SetDrawSize(0,0);
					Circle(0,Draw.Color[Box.SwapPal]);
				break;
				case 4://unfilled circle
					PA_SetDrawSize(0,Box.Tool[Box.ToolSelected].SliderValue);
					UnfilledCircle(0,Draw.Color[Box.SwapPal]);
				break;
				case 5://line tool
					PA_SetDrawSize(0,Box.Tool[Box.ToolSelected].SliderValue);
					LineTool(0,Draw.Color[Box.SwapPal]);
				break;
			}
		break;
		case TB_EyeDropper:
			EyeDropper(0);
			SetCursorXY(Stylus.X,Stylus.Y);
		break;
		case TB_Bucket:
			//bucket undo
			x=((Stylus.X*Draw.Zoom)>>8)+DSBmp.HWScrollX+DSBmp.ScrollX-DSBmp.CanvasX;
			y=((Stylus.Y*Draw.Zoom)>>8)+DSBmp.HWScrollY+DSBmp.ScrollY-DSBmp.CanvasY;	
		
			FFCancel = CreateTextButton(0, 4, 4, (256>>1)-(32>>1), (192>>1)+16,"Cancel",3, 4,PA_RGB(0,0,0));
			CheckFill(x,y,Draw.Color[Box.SwapPal]);
			
			DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
			ReloadMiniView(Settings.MiniviewX,Settings.MiniviewY,Settings.MiniviewWidth,Settings.MiniviewHeight);
			PA_DeleteSprite(FFCancel.Screen,FFCancel.Sprite);
		break;
		case TB_Text:
			PA_SetDrawSize(0,1);
			SelectRectangle(0);
			ShowFileBrowser(FB_IntFont);
		break;
	}
	
}
char Control_List[DrawFuncs][MaxControlChars]=
{
	//main things
	"Scroll",
	"Colors",
	"ColSwap",
	"Undo",
	//start of drawing tools
	"Zoom",
	"Pencil_Square",
	"ColPicker",
	"Bucket",
	"Eraser",
	"Text",
	"Marquis",
	"Square",
	"Square2",
	"Circle",
	"Circle2",
	"Line",
	"Pencil_Circle",
	"Trans_Eraser"
};
u8 *ButtonPushToHold(u8** keypress)
{
	//shoulders
	if(*keypress==&Pad.Newpress.R)
		return &Pad.Held.R;
	else if(*keypress==&Pad.Newpress.L)
		return &Pad.Held.L;
	//face
	else if(*keypress==&Pad.Newpress.A)
		return &Pad.Held.A;
	else if(*keypress==&Pad.Newpress.B)
		return &Pad.Held.B;
	else if(*keypress==&Pad.Newpress.X)
		return &Pad.Held.X;
	else if(*keypress==&Pad.Newpress.Y)
		return &Pad.Held.Y;
	//dpads
	else if(*keypress==&Pad.Newpress.Right)
		return &Pad.Held.Right;
	else if(*keypress==&Pad.Newpress.Left)
		return &Pad.Held.Left;
	else if(*keypress==&Pad.Newpress.Up)
		return &Pad.Held.Up;
	else if(*keypress==&Pad.Newpress.Down)
		return &Pad.Held.Down;
	return NULL;
}
void CustomControls(char *action,u8* buttonHeld)
{
	u8 i=0;
	for(i=0;i<=DrawFuncs;i++)
		if(!strcmp (action,Control_List[i]))break;

	if(i<DrawFuncs)
	{
		switch(i)
		{
			case 0:StylusScrollBg(ButtonPushToHold(&buttonHeld));
			break;
			case 1:PrepColInterface(ButtonPushToHold(&buttonHeld));break;
			//RunColorInterface(ButtonPushToHold(&buttonHeld));break;
			case 2:
				Box.SwapPal++;
				if(Box.SwapPal>1)Box.SwapPal=0;
			break;
			case 3:CheckUndo();break;
			case 4:JumpToTool(TB_Zoom,0);break;
			case 5:JumpToTool(TB_Pencil,1);break;//square pencil
			case 6:JumpToTool(TB_EyeDropper,0);break;
			case 7:JumpToTool(TB_Bucket,0);break;
			case 8:JumpToTool(TB_Eraser,1);break;
			case 9:JumpToTool(TB_Text,0);break;
			case 10:JumpToTool(TB_Marquis,0);break;//square marquis
			case 11:JumpToTool(TB_Shapes,1);break;//filled square
			case 12:JumpToTool(TB_Shapes,2);break;//unfilled square
			case 13:JumpToTool(TB_Shapes,3);break;//filled circle
			case 14:JumpToTool(TB_Shapes,4);break;//unfilled circle
			case 15:JumpToTool(TB_Shapes,5);break;//line
			case 16:JumpToTool(TB_Pencil,2);break;//circle pencil
			case 17:JumpToTool(TB_Eraser,2);break;//trans eraser
		}
	}
}
void Controls(void)
{
	if(Stylus.Held && !GET_FLAG(Box.Flags,TB_State_Created))
		UseTool();
	else if(GET_FLAG(Display.Flag,SCROLL))
		Panning((u8*)Display.Button);
	else if(GET_FLAG(Display.Flag,COLMEN))
		RunColorInterface((u8*)Display.Button);
		
	else if(!Stylus.Held &&!Stylus.Newpress)
	{
		if(Box.Created==0)
		{
			PA_UpdatePad  ();
			DSBmp.UpdateView=1;
			DSBmp.toolused=0;	
			DeleteCursor();
			RESET_FLAG(Box.Flags,TB_State_Hide);
			RunToolBarInterFace();
			if(Settings.FastScroll==1)
			{
				RefreshDrawing(DSBmp.ScrollX, DSBmp.ScrollY,0);
				DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
				DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);	
			}
		}
	}	
}
//RunToolBarInterFace() uses this function
void ToolBoxControls(void)
{
	if(!PA_StylusInZone(Box.X-8,Box.Y,Box.X+16,Box.Y+Box.Length) && Stylus.Newpress)
		Box.Created=0;

	if(Settings.EnableAudio && PA_StylusInZone(Box.MediaX,Box.MediaY,Box.MediaX+128,Box.MediaY+32))
		Box.Created=1;
	//custom controls
	//shoulders
	if(Pad.Newpress.L)
		CustomControls(Custom.L_Button,&Pad.Newpress.L);
	//if(Pad.Newpress.R)CustomControls(Custom.R_Button,&Pad.Newpress.R);
	//dpad
	if(Pad.Newpress.Right)
		CustomControls(Custom.Right_Dpad,&Pad.Newpress.Right);
	if(Pad.Newpress.Left)
		CustomControls(Custom.Left_Dpad,&Pad.Newpress.Left);
	if(Pad.Newpress.Down)
		CustomControls(Custom.Down_Dpad,&Pad.Newpress.Down);
	if(Pad.Newpress.Up)
		CustomControls(Custom.Up_Dpad,&Pad.Newpress.Up);
	//face
	if(Pad.Newpress.A)
		CustomControls(Custom.A_Button,&Pad.Newpress.A);
	if(Pad.Newpress.B)
		CustomControls(Custom.B_Button,&Pad.Newpress.B);
	if(Pad.Newpress.X)
		CustomControls(Custom.X_Button,&Pad.Newpress.X);
	if(Pad.Newpress.Y)
		CustomControls(Custom.Y_Button,&Pad.Newpress.Y);
	
	if(Pad.Newpress.Start)
		SaveMenu();
	if(Pad.Newpress.Select)
		ShowFileBrowser(FB_Image);
	
	if(DSBmp.UpdateView==1)
		TimedReloadMiniView(Settings.UpdateTimer);
	PA_WaitForVBL();
}