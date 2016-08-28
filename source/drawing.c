#include <PA9.h>
#include "bgdata.h"

//============================================================================================================================================================
//Tools
//============================================================================================================================================================
void SaveToolUndo(void)
{
	DC_FlushRange(PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DMA_Copy(PA_DrawBg[0],bg_array[1],24576, DMA_32NOW);  
}
void UndoTool(void)
{
	memcpy((u16*)&bg_array[0],(u16*)&bg_array[1],SCREEN_WIDTH*SCREEN_HEIGHT*2); 
	memset ((bool*)&transLines,0,SCREEN_WIDTH*SCREEN_HEIGHT);
}

void DefineBox(s16 x1,s16 y1,s16 x2, s16 y2, s16 bgx1, s16 bgy1, s16 bgx2, s16 bgy2)
{
	s16 tempx, tempy;
	if(x2<x1)
	{
		tempx=x2;x2=x1;x1=tempx;
		Draw.X1 = x1-((DSBmp.ScrollX*Draw.Zoom)>>8)-DSBmp.HWScrollX;
		Draw.X2 = x2-((DSBmp.ScrollX*Draw.Zoom)>>8)-DSBmp.HWScrollX;
		tempx=bgx2;bgx2=bgx1;bgx1=tempx;
		Draw.BGX1 = bgx1;
		Draw.BGX2 = bgx2;
	}
	else 
	{
		Draw.X1 = x1-((DSBmp.ScrollX*Draw.Zoom)>>8)-DSBmp.HWScrollX;
		Draw.X2 = x2-((DSBmp.ScrollX*Draw.Zoom)>>8)-DSBmp.HWScrollX;
		Draw.BGX1 = bgx1;
		Draw.BGX2 = bgx2;
	}
	if(y2<y1)
	{
		tempy=y2;y2=y1;y1=tempy;
		Draw.Y1 = y1-((DSBmp.ScrollY*Draw.Zoom)>>8)-DSBmp.HWScrollY;
		Draw.Y2 = y2-((DSBmp.ScrollY*Draw.Zoom)>>8)-DSBmp.HWScrollY;
		tempy=bgy2;bgy2=bgy1;bgy1=tempy;
		Draw.BGY1 = bgy1;
		Draw.BGY2 = bgy2;
	}		
	else
	{
		Draw.Y1 = y1-((DSBmp.ScrollY*Draw.Zoom)>>8)-DSBmp.HWScrollY;
		Draw.Y2 = y2-((DSBmp.ScrollY*Draw.Zoom)>>8)-DSBmp.HWScrollY;
		Draw.BGY1 = bgy1;
		Draw.BGY2 = bgy2;
	}
}
void UpdateToolFin(function rawr, u8 a, u16 b,u16 c,u16 d, u16 e, u16 f,u8 g,bool h)
{
	UndoTool();
	rawr(a,b,c,d,e,f,g,h);
	if(Settings.FastScroll==1)
	{
		RefreshDrawing(DSBmp.ScrollX, DSBmp.ScrollY,0);
		DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);	
	}
	DSBmp.UpdateView=1;
}
void Fill16bitBlock(s16 x,s16 y,u32 height,u32 width, u16 color,u16* buffer)
{
	int i=0;
	s16 TempWidth=width;
	s16 OffsetX=0;
	if(x+width >SCREEN_WIDTH)
		TempWidth=width-(x+width-SCREEN_WIDTH);
		
	else if(x<0)
	{
		OffsetX=abs(x);
		TempWidth=width+x;
	}
	if(TempWidth>0)
	{
		for(i=0;i<height;++i)
		{
			if(y+i<SCREEN_HEIGHT && y+i>0)
			{
				int INDEX = SCREEN_INDEX(OffsetX+x,y+i);
				DC_FlushRange((u16*)&buffer[INDEX],TempWidth*2);
				dmaFillHalfWords (color, (u16*)&buffer[INDEX],TempWidth*2);
			}
		}
	}
}
u16 ColorBrightness(u16 color, s8 brightness)
{
	//max brightness is 93 (31+31+31)
	int red=color&31;
	int green=color>>5&31;
	int blue=color>>10&31;
	int newbrightness = red+green+blue;
	
	while (newbrightness>=brightness)
	{
		if(red>0)--red;
		if(green>0)--green;
		if(blue>0)--blue;
		newbrightness = red+green+blue;
		if(blue==0 && red==0 && green==0)
			break;
	}
	while (newbrightness<brightness)
	{
		if(red<31)++red;
		if(green<31)++green;
		if(blue<31)++blue;
		newbrightness = red+green+blue;
		if(blue==31 && red==31 && green==31)
			break;		
	}
	return PA_RGB(red,green,blue);
}
inline u16 BAG_GetPixel(u8 screen, s16 x, s16 y)
{
	if(x>=0 &&x< SCREEN_WIDTH && y>=0 && y<SCREEN_HEIGHT)
		return bg_array[0][SCREEN_INDEX(x,y)];
	return 0;
}
u16 BAG_GetLargeBgPixel(u8 screen, s16 x, s16 y, s8 all_layers)
{
	if(all_layers== 0 && x>=0 && x<DSBmp.Width && y>=0 && y<DSBmp.Height)
		return bglayer[LayerOrder[DSBmp.LayerSelected]][CANVAS_INDEX(x,y,DSBmp.Width)];
		
	else if(all_layers && x>=0 && x<DSBmp.Width && y>=0 && y<DSBmp.Height)
	{
		int i=0;
		for(i=DSBmp.NumOfLayers;i>=0;i--)
		{
			if(i != DSBmp.LayerSelected)
			{
				u16 TempColor =bglayer[LayerOrder[i]][CANVAS_INDEX(x,y,DSBmp.Width)];
				if(TempColor>=TransLim)
					return TempColor;
			}
		}
	}

	return 0;
}
u16 Draw_GetLargeBgPixel(u8 screen, s16 x, s16 y, s8 all_layers)
{
	if(all_layers== 0 && x>=0 && x<DSBmp.Width && y>=0 && y<DSBmp.Height)
		return bglayer[LayerOrder[DSBmp.LayerSelected]][CANVAS_INDEX(x,y,DSBmp.Width)];
		
	else if(all_layers && x>=0 && x<DSBmp.Width && y>=0 && y<DSBmp.Height)
	{
		int i=0;
		for(i=DSBmp.LayerSelected;i>=0;i--)
		{
			u16 TempColor =bglayer[LayerOrder[i]][CANVAS_INDEX(x,y,DSBmp.Width)];
			if(TempColor>=TransLim)
				return TempColor;
		}
	}

	return 0;
}
u16 Draw_GetIndexLargeBgPixel(u8 screen, int index, s8 all_layers, s8 start_layer)
{
	if(all_layers== 0 && index < DSBmp.Height * DSBmp.Width * 2)
		return bglayer[LayerOrder[start_layer]][index];
		
	else if(all_layers && index < DSBmp.Height * DSBmp.Width * 2 )
	{
		int i=0;
		for(i=start_layer;i>=0;i--)
		{
			u16 TempColor =bglayer[LayerOrder[i]][index];
			if(TempColor>=TransLim)
				return TempColor;
		}
	}

	return TransLim;
}
u16 TransparentColor(bool screen,u16 color, s16 transp, u16 Color2)
{ // transp in .8 fixed point
	if(DSBmp.Bits>8 && transp<255 && color >=TransLim )
	{//if there is transparency
		//current color chosen to draw with
		u32 red=color&31;
		u32 green=(color>>5)&31;
		u32 blue=(color>>10)&31;

		u32 NewRed1=(red)*transp>>8;
		u32 NewGreen1=(green)*transp>>8;
		u32 NewBlue1=(blue)*transp>>8;

		u32 red2=Color2&31;
		u32 green2=Color2>>5&31;
		u32 blue2=Color2>>10&31;

		u32 NewRed2=(red2)*(256-transp)>>8;
		u32 NewGreen2=(green2)*(256-transp)>>8;
		u32 NewBlue2=(blue2)*(256-transp)>>8;
		//final color
		return PA_RGB((NewRed1+NewRed2),(NewGreen1+NewGreen2),(NewBlue1+NewBlue2)); 
	}
	return color;
}
u16 ReverseAlpha (u16 bg_col,u16 fg_col, u16 alpha)
{
	if(bg_col <TransLim)
		return fg_col;
	if(fg_col <TransLim)
		return bg_col;
	u32 red=bg_col&31;
	u32 green=(bg_col>>5)&31;
	u32 blue=(bg_col>>10)&31;
		
	u32 red2=fg_col&31;
	u32 green2=fg_col>>5&31;
	u32 blue2=fg_col>>10&31;

	u32 finalR = ((red2 - red)*alpha)/(256-alpha);
	//((red2 *256)-(red*alpha))/ (256-alpha);
	u32 finalG =  ((green2 - green)*alpha)/(256-alpha);
	//((green2 *256)-(green*alpha))/ (256-alpha);
	u32 finalB =  ((blue2 - blue)*alpha)/(256-alpha);
	//((blue2 *256)-(blue*alpha))/ (256-alpha);
	return (PA_RGB ( finalR, finalG,finalB));
}


void AddAlpha(u8 DrawLayer,u16 alpha,int x, int y)
{
	if(DrawLayer>0
	){
		if(alpha>256)
			alpha=255;
		int index= CANVAS_INDEX(x,y,DSBmp.Width);
		alphalayer[DrawLayer-1][index] = alpha;
	}
}
u8 GetAlpha(u8 DrawLayer, int x, int y)
{
	if(DrawLayer>0)
	{
		int index= CANVAS_INDEX(x,y,DSBmp.Width);
		return alphalayer[DrawLayer-1][index];
	}
	return 0;
}
u8 GetAlphaIndex(u8 DrawLayer, int index)
{
	if(DrawLayer>0)
		return alphalayer[DrawLayer-1][index];
		
	return 0;
}	
//============================================================================================================================================================
//Plot Pixel
//============================================================================================================================================================
inline bool PixelOnScreen(s16 x, s16 y)
{
	if(y>=0 && y<SCREEN_HEIGHT && x>=0 && x<SCREEN_WIDTH)
		return 1;
		
	return 0;
}
inline bool PixelInUndoRange(s16 x, s16 y)
{
	s16 StartX =DSBmp.ScrollX-((UNDO_Width>>1)-(SCREEN_WIDTH>>1)); 
	s16 StartY =DSBmp.ScrollY-((UNDO_Height>>1)-(SCREEN_HEIGHT>>1)); 
	if(x>=StartX  && x<= StartX + UNDO_Width-1 && y>=StartY && y<=StartY+UNDO_Height)
		return 1;
		
	return 0;
}

void BAG_TempScreenPixel(s16 x,s16 y,u16 color)
{
	s16 NewScrollX = (DSBmp.ScrollX*Draw.Zoom)>>8;
	s16 NewScrollY = (DSBmp.ScrollY*Draw.Zoom)>>8;

	s16 ScreenX=x-NewScrollX+DSBmp.CanvasX;
	s16 ScreenY=y-NewScrollY+DSBmp.CanvasY;
	
	if(ScreenX<SCREEN_WIDTH && ScreenX>=0 && ScreenY<SCREEN_HEIGHT && ScreenY>=0)
	{
		if(transLines[ScreenX + (ScreenY *SCREEN_WIDTH)]==0)
		{
			u32 NewColor= TransparentColor(0,color,Draw.Alpha,BAG_GetPixel(0,ScreenX ,ScreenY));
			transLines[ScreenX + ((ScreenY) *SCREEN_WIDTH)]=1;
			if(DSBmp.Bits==8)
				bg_array[0][ScreenX + ((ScreenY) *SCREEN_WIDTH)]=palette[color];
			else 
				bg_array[0][ScreenX + ((ScreenY) *SCREEN_WIDTH)]=NewColor;
		}
	}
}
u16 Calc8BitDisplay(u32 index, u16 Color)
{
	if(DSBmp.NumOfLayers > 0)
	{
		int i=0;
		for(i = DSBmp.NumOfLayers;i>=0;i--)
		{
			u16 TestCol = bglayer[LayerOrder[i]][index];
			if(TestCol<Trans8bit)
				return TestCol;
		}
		
	}
	return Color;
}
		
void _Put8BitPixel(int x, int y, int color)
{
	if(x<DSBmp.Width && x >=0 && y >= 0 && y <DSBmp.Height)
	{
		if(PixelInUndoRange(x,y ))
		{
			int index = CANVAS_INDEX(x,y,DSBmp.Width);
			bglayer[LayerOrder[DSBmp.LayerSelected]][index] = color;	
			LinesToWrite[y]=1;
			
			//draw onscreen
			int ScreenX=x-((DSBmp.ScrollX*Draw.Zoom)>>8)+DSBmp.CanvasX;
			int ScreenY=y-((DSBmp.ScrollY*Draw.Zoom)>>8)+DSBmp.CanvasY;			
			if(PixelOnScreen(ScreenX,ScreenY))
			{
				int TempColor = Calc8BitDisplay(index,color);
				bg_array[0][SCREEN_INDEX(ScreenX,ScreenY)]=palette[TempColor];
			}					
		}
	}
}
u16 CalcDisplayPixel(u16 origcol,u16 DrawCol, u16 NewColor,u32 index)
{
	if(DSBmp.NumOfLayers > 0)
	{
		int i=0;
		bool Update[DSBmp.NumOfLayers+1];
		u16 DispBuf[DSBmp.NumOfLayers+1];
		for(i=DSBmp.LayerSelected+1;i<DSBmp.NumOfLayers+1;i++)
		{
			u16 TestCol = bglayer[LayerOrder[i]][index];
			if(TestCol>=TransLim)
			{
				u8 tempA= GetAlphaIndex(i, index);
				if(tempA<255)
				{
					u16 Test = Draw_GetIndexLargeBgPixel(0, index, 1,DSBmp.LayerSelected);
					u16 OldCol = ReverseAlpha (Test,TestCol,tempA);
					DispBuf[i] = TransparentColor(0,origcol,tempA,OldCol);
					DrawCol = DispBuf[i];
					Update[i]=1;
				}
				else 
					DrawCol =TestCol;
			}
			else 
			{
				DispBuf[i] = NewColor;
				DrawCol  = DispBuf[i];	
			}
		}
		for(i=DSBmp.LayerSelected+1;i<DSBmp.NumOfLayers+1;i++)
		{
			if(Update[i])
				bglayer[LayerOrder[i]][index] = DispBuf[i];
		}

	}
	return DrawCol;
}
u16 CalcErasePixel(u16 origcol,u16 DrawCol, u16 NewColor,u32 index)
{
	if(DSBmp.NumOfLayers > 0 &&  bglayer[LayerOrder[DSBmp.LayerSelected]][index] > TransLim)
	{
		int i=0;
		bool Update[DSBmp.NumOfLayers+1];
		u16 DispBuf[DSBmp.NumOfLayers+1];
		for(i=DSBmp.LayerSelected+1;i<DSBmp.NumOfLayers+1;i++)
		{
			u16 TestCol = bglayer[LayerOrder[i]][index];
			if(TestCol>=TransLim)
			{
				u8 tempA= GetAlphaIndex(i, index);
				if(tempA<255)
				{
					u16 Test = Draw_GetIndexLargeBgPixel(0, index, 1,DSBmp.LayerSelected);
					u16 OldCol = ReverseAlpha (Test,TestCol,tempA);
					u16 NewOldCol = Draw_GetIndexLargeBgPixel(0, index, 1,DSBmp.LayerSelected-1);
					DispBuf[i] = TransparentColor(0,NewOldCol ,tempA,OldCol);
					DrawCol  = DispBuf[i];	
					Update[i]=1;
				}
				else 
					DrawCol =TestCol;
			}
		}
		for(i=DSBmp.LayerSelected+1;i<DSBmp.NumOfLayers+1;i++)
		{
			if(Update[i])
				bglayer[LayerOrder[i]][index] = DispBuf[i];
		}

	}
	return DrawCol;	
}	
u16 TempCalc16Disp(u32 index, u16 Color)
{
	if(DSBmp.NumOfLayers > 0)
	{
		int i=0;
		for(i = DSBmp.NumOfLayers;i>=0;i--)
		{
			if(i!=DSBmp.LayerSelected)
			{
				u16 TestCol = bglayer[LayerOrder[i]][index];
				if(TestCol>=TransLim)
					return TestCol;
			}
		}
		
	}
	return Color;
}
void _Put16BitPixel(int x, int y, int color, int alpha, int disableAlphaCheck)
{
	if(x<DSBmp.Width && x >=0 && y >= 0 && y <DSBmp.Height)
	{
		int ScreenX=(x-DSBmp.ScrollX)+DSBmp.CanvasX;
		int ScreenY=(y-DSBmp.ScrollY)+DSBmp.CanvasY;
		
		if(Box.ToolSelected==TB_Eraser)
			alpha=256;
		if(PixelInUndoRange(x ,y ))
		{
			int NewColor= TransparentColor(0,color,alpha,Draw_GetLargeBgPixel(0, x,y, 1));
			LinesToWrite[y]=1;
			int index = CANVAS_INDEX(x,y,DSBmp.Width);
			if(PixelOnScreen(ScreenX,ScreenY))
			{	
				int INDEX = SCREEN_INDEX(ScreenX,ScreenY);
				if((transLines[INDEX]==0 || disableAlphaCheck==1))
				{
					transLines[INDEX]=1;		
					u16 TempColor =TempCalc16Disp(index, NewColor);
					/*NewColor;
					if(color>=TransLim)TempColor = CalcDisplayPixel(color,TempColor, NewColor,index);
					else TempColor = CalcErasePixel(color,TempColor, NewColor,index);
				
					bglayer[LayerOrder[DSBmp.LayerSelected]][index] = NewColor;
					AddAlpha(DSBmp.LayerSelected,alpha,x, y);*/
					bg_array[0][INDEX]=TempColor;
					bglayer[LayerOrder[DSBmp.LayerSelected]][index] = NewColor;
				}
			}
			else 
				bglayer[LayerOrder[DSBmp.LayerSelected]][CANVAS_INDEX(x,y,DSBmp.Width)] = NewColor;
		}
	}
}

void BAG_Put16bitPixel(u8 screen, s16 x, s16 y, u16 color,bool Blitz,u16 alpha,bool disableAlphaCheck) 
{
	if(Blitz)
		BAG_TempScreenPixel(x,y,color);
	else
	{
		if(screen==0)
		{
			s16 NewScrollX = (DSBmp.ScrollX*Draw.Zoom)>>8;
			s16 NewScrollY = (DSBmp.ScrollY*Draw.Zoom)>>8;
			//by default fill in pixel values that are off screen
			switch(DSBmp.Bits)
			{
				case 8:
					_Put8BitPixel(x+DSBmp.ScrollX-NewScrollX , y + DSBmp.ScrollY-NewScrollY, color);
				break;
				default:
					_Put16BitPixel(x+DSBmp.ScrollX-NewScrollX,y + DSBmp.ScrollY-NewScrollY, color, alpha, disableAlphaCheck);
				break;
			}
		}
		else if(x>0 && x<SCREEN_WIDTH && y>0 && y<SCREEN_HEIGHT)
			bg_array[screen][SCREEN_INDEX(x,y)]=color;
	}
}
//============================================================================================================================================================
//Line Drawing
//============================================================================================================================================================
// THE EXTREMELY FAST LINE ALGORITHM Variation E (Addition Fixed Point PreCalc)
void FastLine(u8 screen, s16 x, s16 y, s16 x2, s16 y2,u16 color,bool Blitz) 
{
   	bool yLonger=false;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen))
	{
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;				
		yLonger=true;
	}
	int decInc,j=0;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) 
	{
		if (longLen>0) 
		{
			longLen+=y;
			for (j=0x8000+(x<<16);y<=longLen;++y) 
			{
				BAG_Put16bitPixel(screen,j >> 16,y, color,Blitz,Draw.Alpha,0);
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (j=0x8000+(x<<16);y>=longLen;--y) 
		{
			BAG_Put16bitPixel(screen,j >> 16,y, color,Blitz,Draw.Alpha,0);			
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) 
	{
		longLen+=x;
		for (j=0x8000+(y<<16);x<=longLen;++x) {
			BAG_Put16bitPixel(screen,x,j >> 16, color,Blitz,Draw.Alpha,0);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (j=0x8000+(y<<16);x>=longLen;--x) 
	{
		BAG_Put16bitPixel(screen,x,j >> 16, color,Blitz,Draw.Alpha,0);
		j-=decInc;
	}
}

void FastLineEx(u8 screen, s16 basex, s16 basey, s16 endx, s16 endy, u16 color, s8 size,bool Blitz)
{
	s8 low = (size >> 1) - size +1;
	s8 high = (size >> 1)+1;
	s16 i, j;
	s16 x1, x2, y1, y2;

	for (i = low; i < high; ++i)
	{
		for (j = low; j < high; ++j)
		{
			if ((basex+i >= 0) && (basey+j >= 0)&&(basex+i < DSBmp.Width-1) && (basey+j < DSBmp.Height-1))
				BAG_Put16bitPixel(screen, basex+i, basey+j, color,Blitz,Draw.Alpha,0);
		}
	}
	for (i = low; i < high;++i)
	{
		j = low;
		x1 = basex+i; x2 = endx+i; y1 = basey+j; y2 = endy+j;
		while(x1 < 0) ++x1;	while(x1 > DSBmp.Width-1) --x1;
		while(x2 < 0) ++x2;	while(x2 > DSBmp.Width-1) --x2;
		while(y1 < 0) ++y1;	while(y1 > DSBmp.Height-1) --y1;
		while(y2 < 0) ++y2;	while(y2 > DSBmp.Height-1) --y2;		
		FastLine(screen, x1, y1, x2, y2, color,Blitz);
			
		j = high-1;
		x1 = basex+i; x2 = endx+i; y1 = basey+j; y2 = endy+j;
		while(x1 < 0) ++x1;	while(x1 > DSBmp.Width-1) --x1;
		while(x2 < 0) ++x2;	while(x2 > DSBmp.Width-1) --x2;
		while(y1 < 0) ++y1;	while(y1 > DSBmp.Height-1) --y1;
		while(y2 < 0) ++y2;	while(y2 > DSBmp.Height-1) --y2;
		FastLine(screen, x1, y1, x2, y2, color,Blitz);		
	}

	for (j = low; j < high; j++)
	{
		i = low;
		x1 = basex+i; x2 = endx+i; y1 = basey+j; y2 = endy+j;
		while(x1 < 0) ++x1;	while(x1 > DSBmp.Width-1) --x1;
		while(x2 < 0) ++x2;	while(x2 > DSBmp.Width-1) --x2;
		while(y1 < 0) ++y1;	while(y1 > DSBmp.Height-1) --y1;
		while(y2 < 0) ++y2;	while(y2 > DSBmp.Height-1) --y2;	
		FastLine(screen, x1, y1, x2, y2, color,Blitz);
		i = high-1;
		x1 = basex+i; x2 = endx+i; y1 = basey+j; y2 = endy+j;
		while(x1 < 0) ++x1;	while(x1 > DSBmp.Width-1) --x1;
		while(x2 < 0) ++x2;	while(x2 > DSBmp.Width-1) --x2;
		while(y1 < 0) ++y1;	while(y1 > DSBmp.Height-1) --y1;
		while(y2 < 0) ++y2;	while(y2 > DSBmp.Height-1) --y2;	
		FastLine(screen, x1, y1, x2, y2, color,Blitz);		
	}

}
void LineTool(u8 screen,u16 color)
{	
	s16 NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
	s16 NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
	Stylus.oldx[screen] = NewX; Stylus.oldy[screen] = NewY;
	SaveToolUndo();
	while(Stylus.Held)
	{
		UndoTool();
		NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
		NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
		FastLineEx(screen, Stylus.oldx[screen], Stylus.oldy[screen],NewX ,NewY, color, PA_drawsize[screen],1);
		
		DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
		PA_WaitForVBL();
	}

	function pf;
	pf=(void*)&FastLineEx;
	UpdateToolFin(pf,screen, NewX, NewY, Stylus.oldx[screen], Stylus.oldy[screen], color, PA_drawsize[screen],0);
}
//============================================================================================================================================================
//Pencil Tool
//============================================================================================================================================================
void BAG_16bitDraw(u8 screen,s16 x,s16 y ,u16 color,bool blit)
{
	s16 NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
	s16 NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
	if (Stylus.Newpress) 
	{
		s16 low = (PA_drawsize[screen] >> 1) - PA_drawsize[screen]+1;
		s16 high = (PA_drawsize[screen] >> 1)+1;
		s16 i =0, j = 0;

		for (i = low; i < high;++ i)
			for (j = low; j < high; ++j)
				BAG_Put16bitPixel(screen, NewX+i, NewY+j, color,blit,Draw.Alpha,0);
	}
	else if (Stylus.Held) 
	{
		if(Stylus.olddowntime[screen] != (Stylus.Downtime-1)){
			Stylus.oldx[screen] = NewX;
			Stylus.oldy[screen] = NewY;
		}
		FastLineEx(screen, NewX, NewY, Stylus.oldx[screen], Stylus.oldy[screen], color, PA_drawsize[screen],blit);
	}
	Stylus.oldx[screen] = NewX; Stylus.oldy[screen] = NewY; Stylus.olddowntime[screen] = Stylus.Downtime;
}
inline void DrawSizePixel(u8 screen, s16 x, s16 y, u16 color,bool blit)
{
	s16 low = (PA_drawsize[screen] >> 1) - PA_drawsize[screen]+1;
	s16 high = (PA_drawsize[screen] >> 1)+1;
	s16 i =0, j = 0;
	for (i = low; i < high;++ i)
		for (j = low; j < high; ++j)
			BAG_Put16bitPixel(screen, x+i, y+j, color,blit,Draw.Alpha,0);
}
//============================================================================================================================================================
//Round Brush Tool
//============================================================================================================================================================
inline void CircleBrush(u8 screen,s16 originx,s16 originy,s16 r,u16 color,u16 alpha,bool alphacheck)
{
	int x=0,y=0;
	s16 NewX=originx;
	s16 NewY=originy;
	r+=2;
	int diameter = (r<<1);
	s16 tempR = r*r, r2=(r-2)*3;
	u16 Brush[(diameter+1)][(diameter+1)];
	if(r-2<=1)r2=4;

	for(x=1;x<=diameter-1;++x)
	{
		for(y=1;y<=diameter-1;++y)
		{
			if(Draw_GetLargeBgPixel(0, NewX+x-r, NewY+y-r,1)!= color)
			{
				s16 CurR = (x-r)*(x-r)+(y-r)*(y-r);
				Brush[x][y]=alpha-227;
				if(CurR<= tempR-r2-1)
					Brush[x][y]=alpha-142;	
				if(CurR <=tempR-r2-2)
					Brush[x][y]=alpha;
			
				if(CurR <=tempR)
					BAG_Put16bitPixel(screen,NewX+x-r,NewY+y-r, color,0,Brush[x][y],(Brush[x][y] == alpha ? 1 : 1));	
			}
		}
	}
}
void FastCircleLine(u8 screen, s16 x, s16 y, s16 x2, s16 y2,u16 color) 
{
	bool yLonger=false;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) 
	{
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;				
		yLonger=true;
	}
	int decInc,j=0;
	if (longLen==0) 
		decInc=0;
	else 
		decInc = (shortLen << 16) / longLen;

	if (yLonger) 
	{
		if (longLen>0) 
		{
			longLen+=y;
			for (j=0x8000+(x<<16);y<=longLen;++y) 
			{
				CircleBrush(screen,j >> 16,y,PA_drawsize[screen], color,Draw.Alpha,0);
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (j=0x8000+(x<<16);y>=longLen;--y) 
		{
			CircleBrush(screen,j >> 16,y,PA_drawsize[screen], color,Draw.Alpha,0);			
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) 
	{
		longLen+=x;
		for (j=0x8000+(y<<16);x<=longLen;++x) 
		{
			CircleBrush(screen,x,j >> 16,PA_drawsize[screen], color,Draw.Alpha,0);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (j=0x8000+(y<<16);x>=longLen;--x)
	{
		CircleBrush(screen,x,j >> 16,PA_drawsize[screen], color,Draw.Alpha,0);
		j-=decInc;
	}
}

void BAG_16bitCircleDraw(u8 screen,s16 x,s16 y ,u16 color)
{
	s16 NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
	s16 NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
	if (Stylus.Newpress) CircleBrush(0,NewX,NewY,PA_drawsize[screen], color,Draw.Alpha,0);
	else if (Stylus.Held) 
	{	
		if(Stylus.olddowntime[screen] != (Stylus.Downtime-1))
		{
			Stylus.oldx[screen] = NewX; 
			Stylus.oldy[screen] = NewY;
		}
		FastCircleLine(0, NewX, NewY,Stylus.oldx[screen], Stylus.oldy[screen],color);
	}
	Stylus.oldx[screen] = NewX; Stylus.oldy[screen] = NewY; Stylus.olddowntime[screen] = Stylus.Downtime;
}
//============================================================================================================================================================
//Rectangle Tool
//============================================================================================================================================================
void BAG_Draw16bitRect(u8 screen, s16 basex, s16 basey, s16 endx, s16 endy, u16 color, bool Blitz)
 {
	s16 i, j;
	if (endx < basex) 
	{
		i = basex;	
		basex = endx;	
		endx = i;
	}
	if (basex < 0) 
		basex = 0;
	if (endx > DSBmp.Width) 
		endx = DSBmp.Width;
		
	s16 lx = endx - basex;
	if (lx) 
	{
		if (endy < basey) 
		{
			i = basey;	
			basey = endy;	
			endy = i;
		}
		if (basey < 0)
			basey = 0;

		if (endy > DSBmp.Height) 
			endy = DSBmp.Height;

		for (j = basey; j < endy; j++) 
			for (i = basex; i < endx; i++) 
				BAG_Put16bitPixel(screen, i, j, color,Blitz,Draw.Alpha,0);
	}
}
void FastRec(u8 screen,u16 *screenbuff,s16 x,s16 y,s16 height,s16 width)
{
	int i=0;
	s16 TempX=x, TempY=y, TempWidth=width;
	if(width<0)
		TempX=x+width;
	if(height<0)
		TempY=y+height;
	
	s16 NewX=TempX-(((DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.CanvasX;
	s16 NewY=TempY-(((DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.CanvasY;
	
	if(NewX+width>SCREEN_WIDTH)
		TempWidth =width- ((NewX+width)-SCREEN_WIDTH);
	if(NewX<0)
	{
		TempWidth=width+NewX;
		NewX-=NewX;
	}
	for(i=0;i<abs(height);++i)
	{	
		if(NewY+i<192)
		{
			int index = SCREEN_INDEX(NewX,NewY+i);
			memcpy ((u16*)&bg_array[0][index],(u16*)&screenbuff[index],abs(TempWidth)*2);
		}
	}
}

void Rectangle(u8 screen,u16 color)
{
	s16 NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
	s16 NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
	Stylus.oldx[screen] = NewX; Stylus.oldy[screen] = NewY;
	SaveToolUndo();
	
	u16 *NewColBuf=NULL;
	NewColBuf= (u16*) calloc (SCREEN_WIDTH*SCREEN_HEIGHT,sizeof(u16));
	if(NewColBuf==NULL)BAG_Error_Print(Error_ShapeAlloc);
	
	if(DSBmp.Bits>=16)
	{
		//scan pixels for transparency
		int x=0,y=0;
		for(x=0;x<SCREEN_WIDTH;++x)
			for(y=0;y<SCREEN_HEIGHT;++y)
				NewColBuf[SCREEN_INDEX(x,y)]=TransparentColor(0,color,Draw.Alpha,BAG_GetPixel(0,x ,y));
	}
	else if(DSBmp.Bits==8)
	{
		int x=0,y=0;
		for(x=0;x<SCREEN_WIDTH;++x)
			for(y=0;y<SCREEN_HEIGHT;++y)
				NewColBuf[SCREEN_INDEX(x,y)]=palette[color];
	}
	
	while(Stylus.Held)
	{
		UndoTool();
		NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
		NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
		FastRec(0,NewColBuf,Stylus.oldx[screen],Stylus.oldy[screen],NewY-Stylus.oldy[screen],NewX-Stylus.oldx[screen]);
		DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
	}
	free(NewColBuf);
	NewColBuf=NULL;
	function pf;
	pf=(void*)&BAG_Draw16bitRect;
	UpdateToolFin(pf,screen,Stylus.oldx[screen],Stylus.oldy[screen],NewX,NewY, color,0,0);
}
//============================================================================================================================================================
//Line Rectangle
//============================================================================================================================================================
void FastLineRec(u8 screen,u16 *screenbuff,s16 x,s16 y,s16 height,s16 width)
{
	s16 NewX=x,NewY=y;
	if(width<0)
		NewX-=abs(width);
	if(height<0)
		NewY-=abs(height);
	//top line
	FastRec(screen,screenbuff,NewX,NewY,PA_drawsize[screen],abs(width));
	//bottom line
	FastRec(screen,screenbuff,NewX,NewY+abs(height),PA_drawsize[screen],abs(width));
	//left line
	FastRec(screen,screenbuff,NewX,NewY,abs(height),PA_drawsize[screen]);
	//right line
	FastRec(screen,screenbuff,NewX+abs(width),NewY,abs(height)+PA_drawsize[screen],PA_drawsize[screen]);
}
void BAG_Draw16bitLineRect(u8 screen, s16 basex, s16 basey, s16 endx, s16 endy, u16 color, bool Blitz) 
{
	s16 NewX=basex,NewY=basey;
	if(endx<basex)
	{
		NewX=basex;
		basex=endx;
		endx=NewX;
	}
	if(endy<basey)
	{
		NewY=basey; 
		basey=endy;
		endy=NewY;
	}
	//top
	BAG_Draw16bitRect(screen,basex,basey,endx,basey+PA_drawsize[screen],color,Blitz); 
	//bottom
	BAG_Draw16bitRect(screen,basex,endy,endx+PA_drawsize[screen],endy+PA_drawsize[screen],color,Blitz); 
	//left
	BAG_Draw16bitRect(screen,basex,basey,basex+PA_drawsize[screen],endy,color,Blitz); 
	//right
	BAG_Draw16bitRect(screen,endx,basey,endx+PA_drawsize[screen],endy,color,Blitz); 
}
void LineRectangle(u8 screen,u16 color)
{
	s16 NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
	s16 NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
	Stylus.oldx[screen] = NewX; Stylus.oldy[screen] = NewY;
	SaveToolUndo();
	
	u16 *NewColBuf=NULL;
	NewColBuf= (u16*) calloc (SCREEN_WIDTH*SCREEN_HEIGHT,sizeof(u16));
	if(NewColBuf==NULL)BAG_Error_Print(Error_ShapeAlloc);
	
	if(DSBmp.Bits>=16)
	{
		//scan pixels for transparency
		int x=0,y=0;
		for(x=0;x<SCREEN_WIDTH;++x)
			for(y=0;y<SCREEN_HEIGHT;++y)
			NewColBuf[SCREEN_INDEX(x,y)]=TransparentColor(0,color,Draw.Alpha,BAG_GetPixel(0,x ,y));
	}
	else if(DSBmp.Bits==8)
	{
		int x=0,y=0;
		for(x=0;x<SCREEN_WIDTH;++x)
			for(y=0;y<SCREEN_HEIGHT;++y)
				NewColBuf[SCREEN_INDEX(x,y)]=palette[color];
	}
	while(Stylus.Held)
	{
		UndoTool();
		NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
		NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
		FastLineRec(0,NewColBuf,Stylus.oldx[screen],Stylus.oldy[screen],NewY-Stylus.oldy[screen],NewX-Stylus.oldx[screen]);
		DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
	}
	free(NewColBuf);
	NewColBuf=NULL;
	function pf;
	pf=(void*)&BAG_Draw16bitLineRect;
	UpdateToolFin(pf,screen,Stylus.oldx[screen],Stylus.oldy[screen],NewX,NewY, color,0,0);
}
//============================================================================================================================================================
//Selector Tool
//============================================================================================================================================================
extern u8 CheckZoomValue(s32 zoom, s8 limit);

void LineRect16c(s8 screen, s16 x1, s16 y1, s16 x2, s16 y2)
{
	//Check grids first before drawing
	s32 tempGridVal=0;
	if(GET_FLAG(Settings.GridFlags,GRID_ON) && GET_FLAG(Settings.GridFlags,GRID_ZOOMED))
		tempGridVal = CheckZoomValue(Draw.Zoom,3);
	if(GET_FLAG(Settings.GridFlags,GRID_ON) && !GET_FLAG(Settings.GridFlags,GRID_ZOOMED))
		tempGridVal = CheckZoomValue(Box.Tool[TB_Grid].SliderValue,5);
	if(tempGridVal!=0)
	{
		s16 GridVX=tempGridVal, GridVY=tempGridVal;
		if(GridVX>=128)
			GridVY=96;
		if(GET_FLAG(Settings.GridFlags,GRID_ZOOMED))
			Draw16cGrid(0,256/tempGridVal,256/GridVY,PA_RGB(31,31,31),PA_RGB(0,0,31));
		else 	
			Draw16cGrid(0,tempGridVal,GridVY,PA_RGB(31,31,31),PA_RGB(0,0,31));
	}	
	else 
		PA_Init16cBg  (screen, Layer_Grid);
	
	s16 i=0;
	s16 TempCols[2]=
	{
		3,
		4,
	};
	PA_SetBgPalCol(screen,(0<<4)+3,PA_RGB(31,31,31)); 
	PA_SetBgPalCol(screen,(0<<4)+4,PA_RGB(31,0,0)); 
	//ensure lines are all drawn 
	if (x2 < x1) 
	{
		i = x1;	
		x1 = x2;	
		x2 = i;
	}
	if (x1 < 0) 
		x1 = 0;
	if (x2 > SCREEN_WIDTH)
		x2 = SCREEN_WIDTH;

	if (y2 < y1) 
	{
		i = y1;	
		y1 = y2;	
		y2 = i;
	}
	if (y1 < 0) 
		y1 = 0;
	if (y2 > SCREEN_HEIGHT) 
		y2 = SCREEN_HEIGHT;
	
	s8 PixelColor=1;
	//horizontal lines
	for(i=x1;i<x2;++i)
	{
		PA_16cPutPixel  (screen,i,y1,TempCols[PixelColor]);
		PA_16cPutPixel  (screen,i,y2,TempCols[PixelColor]);
		++PixelColor;
		if(PixelColor>1)
			PixelColor=0;
	}
	i=0;
	PixelColor=1;
	//vertical lines
	for(i=y1;i<y2;++i)
	{
		PA_16cPutPixel  (screen,x1,i,TempCols[PixelColor]);
		PA_16cPutPixel  (screen,x2,i,TempCols[PixelColor]);
		++PixelColor;
		if(PixelColor>1)
			PixelColor=0;
	}
}
			
void SelectRectangle(u8 screen)
{
	s16 NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX;
	s16 NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY;
	s16 NewOldX=NewX;
	s16 NewOldY=NewY;
	s16 TempX = Stylus.X, TempY = Stylus.Y;
	

	while(Stylus.Held)
	{
		NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX;
		NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY;
		LineRect16c(0,TempX,TempY,Stylus.X,Stylus.Y);
		PA_WaitForVBL();	
	}
	DefineBox(NewOldX,NewOldY,NewX,NewY,TempX,TempY,Stylus.X,Stylus.Y);
	SaveToolUndo();
}
void ClearSelectBox(u8 screen)
{
	PA_16cErase(screen);
	SET_FLAG(Settings.GridFlags,GRID_DELETED);
	CheckGrid();
}
//============================================================================================================================================================
//Unfilled Circle
//============================================================================================================================================================
void CirclePixel(s16 x,s16 y,s16 height,s16 width, u16 *color,u16* buffer)
{
	int i=0;
	s16 TempWidth=width;
	s16 OffsetX=0;
	if(x+width >SCREEN_WIDTH)
		TempWidth=width-(x+width-SCREEN_WIDTH);

	else if(x<0)
	{
		OffsetX=abs(x);
		TempWidth=width+x;
	}
	if(TempWidth>0)
	{
		for(i=0;i<height;++i)
		{
			if(y+i<192 && y+i>0)
			{
				int index = SCREEN_INDEX(OffsetX+x,y+i);
				memcpy ( (u16*)&buffer[index],(u16*)&color[index],TempWidth*2);
			}
		}
	}
}

inline void CirclePosCheck(s8 screen, s16 x, s16 y,u16 color,bool blit)
{
	DrawSizePixel(screen,x,y ,color,blit);
}

void DrawUnfilledCircle(s8 screen,s16 x0, s16 y0, s16 radius,s16 nothing,u16 color,s16 nothing2, bool blit)
{
	if(radius<0)
	{
		x0-=radius<<1;
		radius=abs(radius);
	}
	s16 f = 1 - radius;
	s16 ddF_x = 0;
	s16 ddF_y = -2 * radius;
	s16 x = -radius;
	s16 y = radius;
	s16 XRadiusPos=0;
	s16 offset= PA_drawsize[screen]>>1;
	for(x=0;x < y;++x) 
	{
		if (f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		ddF_x += 2;
		f += ddF_x + 1; 
		
		if(x0>Stylus.oldx[screen])
			XRadiusPos= x0-radius-offset;
		else 
			XRadiusPos= x0+radius-offset;
		CirclePosCheck(screen,XRadiusPos + x,y0 + y-offset,color,blit);
		CirclePosCheck(screen,XRadiusPos - x,y0 + y-offset,color,blit);
		CirclePosCheck(screen,XRadiusPos +  x,y0 - y-offset,color,blit);
		CirclePosCheck(screen,XRadiusPos - x,y0 - y-offset,color,blit);
		CirclePosCheck(screen,XRadiusPos +y,y0 + x-offset,color,blit);
		CirclePosCheck(screen,XRadiusPos -y,y0 + x-offset,color,blit);
		CirclePosCheck(screen,XRadiusPos +y,y0 - x-offset,color,blit);
		CirclePosCheck(screen,XRadiusPos -y,y0 - x-offset,color,blit);
	}
}

void FastUnfilledCircle(s8 screen,s16 x0, s16 y0, s16 radius,u16 *color, bool blit)
{
	if(radius<0)
	{
		x0-=radius<<1;
		radius=abs(radius);
	}
	s16 f = 1 - radius;
	s16 ddF_x = 0;
	s16 ddF_y = -2 * radius;
	s16 x = -radius;
	s16 y = radius;
	s16 XRadiusPos=0;
	s16 offset= PA_drawsize[screen]>>1;
	for(x=0;x < y;++x)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		ddF_x += 2;
		f += ddF_x + 1; 
		
		if(x0>Stylus.oldx[screen])
			XRadiusPos= x0-radius-offset;
		else 
			XRadiusPos= x0+radius-offset;
		CirclePixel(XRadiusPos + x,y0 + y-offset,PA_drawsize[screen],PA_drawsize[screen], color,bg_array[screen]);
		CirclePixel(XRadiusPos - x,y0 + y-offset,PA_drawsize[screen],PA_drawsize[screen], color,bg_array[screen]);
		CirclePixel(XRadiusPos +  x,y0 - y-offset,PA_drawsize[screen],PA_drawsize[screen], color,bg_array[screen]);
		CirclePixel(XRadiusPos - x,y0 - y-offset,PA_drawsize[screen],PA_drawsize[screen], color,bg_array[screen]);
		CirclePixel(XRadiusPos +y,y0 + x-offset,PA_drawsize[screen],PA_drawsize[screen], color,bg_array[screen]);
		CirclePixel(XRadiusPos -y,y0 + x-offset,PA_drawsize[screen],PA_drawsize[screen], color,bg_array[screen]);
		CirclePixel(XRadiusPos +y,y0 - x-offset,PA_drawsize[screen],PA_drawsize[screen], color,bg_array[screen]);
		CirclePixel(XRadiusPos -y,y0 - x-offset,PA_drawsize[screen],PA_drawsize[screen], color,bg_array[screen]);
	}
}

void UnfilledCircle(u8 screen,u16 color)
{
	s16 NewX=((Stylus.X*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
	s16 NewY=((Stylus.Y*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
	Stylus.oldx[screen] = NewX; Stylus.oldy[screen] = NewY;
	
	s16 OffsetX = ((DSBmp.ScrollX * Draw.Zoom)>>8);
	s16 OffsetY =  ((DSBmp.ScrollY * Draw.Zoom)>>8);
	SaveToolUndo();
	
	u16 *NewColBuf=NULL;
	NewColBuf= (u16*) calloc (SCREEN_WIDTH*SCREEN_HEIGHT,sizeof(u16));
	if(NewColBuf==NULL)
		BAG_Error_Print(Error_ShapeAlloc);
	
	if(DSBmp.Bits>=16)
	{
		//scan pixels for transparency
		int x=0,y=0;
		for(x=0;x<SCREEN_WIDTH;++x)
			for(y=0;y<SCREEN_HEIGHT;++y)
				NewColBuf[SCREEN_INDEX(x,y)]=TransparentColor(0,color,Draw.Alpha,BAG_GetPixel(0,x ,y));
	}
	else if(DSBmp.Bits==8)
	{
		int x=0,y=0;
		for(x=0;x<SCREEN_WIDTH;++x)
			for(y=0;y<SCREEN_HEIGHT;++y)
				NewColBuf[SCREEN_INDEX(x,y)]=palette[color];
	}
	while(Stylus.Held)
	{
		UndoTool();
		NewX=((Stylus.X*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
		NewY=((Stylus.Y*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
		FastUnfilledCircle(0,NewX,NewY,NewX-Stylus.oldx[screen],NewColBuf,1);
		DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
	}
	free(NewColBuf);
	NewColBuf=NULL;
	function pf;
	pf=(void*)&DrawUnfilledCircle;
	UpdateToolFin(pf,screen,NewX+((PA_drawsize[screen]-1) >> 1)+OffsetX,NewY+((PA_drawsize[screen]-1) >> 1)+OffsetY,(NewX+OffsetX)-(Stylus.oldx[screen]+OffsetX),0,Draw.Color[Box.SwapPal],0,0);
}
//============================================================================================================================================================
//Filled Circle
//============================================================================================================================================================
void FilledCircle(u8 screen,s16 originx,s16 originy,s16 r,s16 nothing ,u16 color,u8 nothing2,bool Blitz)
{
	int x=0,y=0;
	if(r<0)
	{
		originx-=r<<1;
		r=abs(r);
	}
	s16 NewX=originx-r;
	s16 NewY=originy;
	x=-r;
	for(y=-r+1; y<r; ++y)
		for(x=-r; x<=r; ++x)	
			if(x*x+y*y <= r*r) 
				BAG_Put16bitPixel(screen,NewX+x,NewY+y, color,Blitz,Draw.Alpha,0);
}
void FastFilledCircle(u8 screen,u16* screenbuff,int originx,int originy,int r)
{
	if(r<0)
	{
		originx-=r<<1;
		r=abs(r);
	}
	int x=-r;
	int y=0;
	
	s16 NewX=originx-(((DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.CanvasX;
	s16 NewY=originy-(((DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.CanvasY;
	
	s16 offset=0,offset2=0;
	int bytes=abs((x)<<2);
	
	for(y=-r; y<=r; ++y)
	{
		for(x=-r; x<=0; ++x)
		{
			bytes=abs((x)<<2);
			offset=0;
			offset2=0;
			if(x*x+y*y <= r*r && NewY+y<SCREEN_HEIGHT && NewY+y>=0)
			{// if the first x value of the line is in the circle and on screen
				while((NewX-r)+x<0)
				{//left screen limits
					++x;
					++offset;
					bytes=abs((x)<<2)+(offset<<1);
				}
				while((NewX-r)-x>SCREEN_WIDTH)
				{//right screen limits
					++x;
					offset2--;
					++offset;
					bytes=abs((x)<<2)+(offset<<1);
				}
				//copy one line at time
				int index = SCREEN_INDEX((NewX-r)+(x+offset2),NewY+y);
				memcpy ((u16*)&bg_array[0][index],(u16*)&screenbuff[index],abs(bytes));
				break;
			}
		}
	}
}

void Circle(u8 screen,u16 color)
{
	s16	NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
	s16	NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
	Stylus.oldx[screen] = NewX; Stylus.oldy[screen] = NewY;
	SaveToolUndo();
	
	u16 *NewColBuf=NULL;
	NewColBuf= (u16*) calloc (SCREEN_WIDTH*SCREEN_HEIGHT,sizeof(u16));
	if(NewColBuf==NULL)
		BAG_Error_Print(Error_ShapeAlloc);
	
	if(DSBmp.Bits>=16)
	{
		//scan pixels for transparency
		int x=0,y=0;
		for(x=0;x<SCREEN_WIDTH;++x)	
			for(y=0;y<SCREEN_HEIGHT;++y)
				NewColBuf[SCREEN_INDEX(x,y)]=TransparentColor(0,color,Draw.Alpha,BAG_GetPixel(0,x ,y));
	}
	else if(DSBmp.Bits==8)
	{
		int x=0,y=0;
		for(x=0;x<SCREEN_WIDTH;++x)
			for(y=0;y<SCREEN_HEIGHT;++y)
				NewColBuf[SCREEN_INDEX(x,y)]=palette[color];

	}
	while(Stylus.Held)
	{
		UndoTool();
		NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
		NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
		FastFilledCircle(0,NewColBuf,NewX,NewY,NewX-Stylus.oldx[screen]);
		DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
	}
	free(NewColBuf);
	NewColBuf=NULL;
	function pf;
	pf=(void*)&FilledCircle;
	UpdateToolFin(pf,screen,NewX,NewY,NewX-Stylus.oldx[screen],0,color,0,0);
}
//============================================================================================================================================================
//Bucket tool
//============================================================================================================================================================

inline void FillBmp(s16 x, s16 y, u16 color)
{
	u16 NewColor= TransparentColor(0,color,Draw.Alpha,BAG_GetLargeBgPixel(0,x ,y ,1));
	bglayer[LayerOrder[DSBmp.LayerSelected]][CANVAS_INDEX(x,y,DSBmp.Width)] = NewColor;
	BAG_TempScreenPixel(x,y,color);
	LinesToWrite[y]=1;
}

//bucket tool
inline void AddFill(s16 x, s16 y)
{
	++Bucket.IndexEnd;
	if(Bucket.IndexEnd>Settings.Flood_Depth)Bucket.IndexEnd=0;
	Bucket.nextx[Bucket.IndexEnd] = x;
	Bucket.nexty[Bucket.IndexEnd] = y;   
	++Bucket.nextfill;
	FillBmp(x,y,Bucket.newcolor);
}   

inline void DoFill(s16 x, s16 y){
	if(BAG_GetLargeBgPixel(0, x-1, y,0) == Bucket.oldcolor) 
		AddFill(x-1, y);
	if(BAG_GetLargeBgPixel(0, x+1, y,0) == Bucket.oldcolor)
		AddFill(x+1, y);		
	if(BAG_GetLargeBgPixel(0, x, y-1,0) == Bucket.oldcolor) 
		AddFill(x, y-1);
	if(BAG_GetLargeBgPixel(0, x, y+1,0) == Bucket.oldcolor) 
		AddFill(x, y+1);	
} 

void CheckFill(s32 x, s32 y, u16 color){
	if(x>=0 && x <DSBmp.Width && y>=0 && y<DSBmp.Height)
	{
		s16 newx = x;
		s16 newy = y;
		
		memset(&Bucket, 0,sizeof(Bucket));
		Bucket.oldcolor = BAG_GetLargeBgPixel(0, newx, newy,0);
		Bucket.newcolor = color;

		if(Bucket.oldcolor!=Bucket.newcolor)
		{
			Bucket.nextx=NULL;
			Bucket.nexty=NULL;
			
			Bucket.nextx=(s32*)calloc(Settings.Flood_Depth, sizeof(s32));
			if(Bucket.nextx==NULL)
				BAG_Error_Print(Error_BucketAlloc);
			Bucket.nexty=(s32*)calloc(Settings.Flood_Depth, sizeof(s32));
			if(Bucket.nexty==NULL)
				BAG_Error_Print(Error_BucketAlloc);
				
			AddFill(newx,newy);
			u32 i=0;
			for(i = 0; i <Bucket.nextfill; ++i)
			{
				++Bucket.Index;
				if(Bucket.Index>Settings.Flood_Depth)Bucket.Index=0;
				DoFill(Bucket.nextx[Bucket.Index], Bucket.nexty[Bucket.Index]);
				if(Stylus.Newpress && PA_SpriteTouched(FFCancel.Sprite))break;
			} 
			free(Bucket.nextx);
			free(Bucket.nexty);
		}
	}
} 

//============================================================================================================================================================
//My Font stuff
//============================================================================================================================================================
//use my pixel functions to draw text on the background layer
void BAG_DrawText(s8 screen,s16 x, s16 y, FT_Bitmap *bitmap,bool AA,s16 red,s16 green, s16 blue)
{
    uint8 *src = bitmap->buffer;
	s32 NewX=x;
	s32 NewY=y;	
   // uint16 *dest = bglayer[LayerOrder[DSBmp.LayerSelected]] + NewX + DSBmp.Width * NewY;
    s16 width = bitmap->width / 3;
    s16 height = bitmap->rows;
    s16 src_gap = (bitmap->pitch - bitmap->width);
    s16 x1,y1;
	bool SkipAA=0;

	for(y1=0;y1<height;++y1)
	{
		for (x1=0; x1<width; ++x1) 
		{
			s16 alpha = *(src++);
			src+=2;
			if (alpha) 
			{
				if(AA==0)
				{
					if(alpha<170)
						SkipAA=1;
					else 
						alpha=255;
				}
				if(SkipAA==0)
				{
					//if(NewX+x1>=0 && NewX+x1<NewX+256 && NewY+y1>=0&& NewY+y1<NewY+192){
						u16 color=bglayer[LayerOrder[DSBmp.LayerSelected]][CANVAS_INDEX((NewX+x1),(NewY+y1),DSBmp.Width)];
						s8 testr=color&31;
						s8 testg=(color>>5)&31;
						s8 testb=(color>>10)&31;
						//drawing color
						s8 NewRed1=(red)*alpha>>8;
						s8 NewGreen1=(green)*alpha>>8;
						s8 NewBlue1=(blue)*alpha>>8;

						//get background colors
						s8 NewRed2=(testr)*(256-alpha)>>8;
						s8 NewGreen2=(testg)*(256-alpha)>>8;
						s8 NewBlue2=(testb)*(256-alpha)>>8;
						BAG_Put16bitPixel(0,NewX+x1,NewY+y1,PA_RGB((NewRed1+NewRed2),(NewGreen1+NewGreen2),(NewBlue1+NewBlue2)),0,Draw.Alpha,0);
					//}
				}
				else 
					SkipAA=0;
			}	

		}
		src += src_gap;
    }
}
s8 BAG_draw_string(s8 screen,s16 x1, s16 y1,s16 x2, s16 y2, FT_Face face, const char *string,bool AA,s16 red, s16 green, s16 blue)
{
    int ascent = FT_CEIL(FT_MulFix(face->bbox.yMax, face->size->metrics.y_scale));
    y1 += ascent;
	s32 startX=x1;
	s32 x=x1,y=y1;
	s32 TempX=0;
	s16 i=0;
    for(i=0;string[i]!=0;i++) 
	{
		if(string[i]=='\n')
		{
			y += ascent; 
			x=startX;
		}
		else if(string[i]==' ')
		{
			x += FT_CEIL(face->glyph->metrics.horiAdvance);
			if(x>x2)
			{//if text is going off screen, move down a line
				y += ascent; 
				x=startX;
			}
		}
		else
		{		
			FT_Load_Char(face, string[i], FT_LOAD_RENDER | FT_LOAD_TARGET_LCD);
				if(x + FT_CEIL(face->glyph->metrics.horiAdvance)>x2)
				{//if text is going off screen, move down a line
					y += ascent;					
					x=startX;
				}
				if(y>y2)
					return i;
				TempX=x + FT_FLOOR(face->glyph->metrics.horiBearingX);				
				while(TempX<x1)
				{
					x++;
					TempX= x + FT_FLOOR(face->glyph->metrics.horiBearingX);
				}
					BAG_DrawText(screen,TempX,
								y - FT_FLOOR(face->glyph->metrics.horiBearingY),
								&face->glyph->bitmap,AA,red, green, blue);
			x += FT_CEIL(face->glyph->metrics.horiAdvance);
		}
		
    }
	return i;//return how many characters displayed
}

s8 BAG_OutputText(s8 screen,s16 x1,s16 y1,s16 x2, s16 y2,const char *string,s8 AA,int fontsize,s8 slot,u16 color)
{
	s8 red=color&31;
	s8 green=color>>5&31;
	s8 blue=color>>10&31;
	FT_Set_Char_Size(ft_face[slot], 0, fontsize, 0, 0);
	x1+=(((DSBmp.ScrollX)*Draw.Zoom)>>8)-DSBmp.HWScrollX-DSBmp.CanvasX;
	x2+=(((DSBmp.ScrollX)*Draw.Zoom)>>8)-DSBmp.HWScrollX-DSBmp.CanvasX;
	y1+=(((DSBmp.ScrollY)*Draw.Zoom)>>8)-DSBmp.HWScrollY-DSBmp.CanvasY;
	y2+=(((DSBmp.ScrollY)*Draw.Zoom)>>8)-DSBmp.HWScrollY-DSBmp.CanvasY;

	return (BAG_draw_string(screen,x1,y1,x2,y2,ft_face[slot],string,AA,red,green,blue));
}
//============================================================================================================================================================
//EyeDropper tool
//============================================================================================================================================================
void EyeDropper(u8 screen)
{
	s16 NewX=(((Stylus.X+DSBmp.ScrollX)*Draw.Zoom)>>8)+DSBmp.HWScrollX-DSBmp.CanvasX;
	s16 NewY=(((Stylus.Y+DSBmp.ScrollY)*Draw.Zoom)>>8)+DSBmp.HWScrollY-DSBmp.CanvasY;
	Draw.Color[Box.SwapPal]=Draw_GetLargeBgPixel(0,NewX,NewY,1);
	UpdateColorSquares(Box.SwapPal);
}
//============================================================================================================================================================
//Zoom Tool
//============================================================================================================================================================
u8 CheckZoomValue(s32 zoom, s8 limit)
{
	u8 GridLevels[5]={16,32,64,//zooms with possible grids
	8,128};//extra possible grid sizes (8x8 pixels etc)
	s8 i=0;
	for(i=0;i<limit;i++)
		if(zoom >GridLevels[i]-5 && zoom <GridLevels[i]+5)
			return GridLevels[i];
			
	return 0;
}
u8 CheckGridValue(s8 zoom)
{
	u8 GridLevels[6]={0,8,16,32,64,128};//extra possible grid sizes (8x8 pixels etc)
	return GridLevels[zoom];
}

void UpdateZoom(u8 tool)
{
	if(Stylus.Held && PA_StylusInZone(Box.X,Box.Y+42,Box.X+16,Box.Y+40+92) && Stylus.X !=Stylus.oldx[0] && Stylus.Y != Stylus.oldy[0])
	{
		bool UpdateDrawing=0;
		//draw size slider
		if(tool==TB_Grid)
		{
			s32 GridVal= Box.Tool[tool].SliderValue;
			s32 tempGridVal=CheckGridValue(GridVal);
			if(tempGridVal!=0)
			{
				GridVal=tempGridVal;
				s16 GridVX=tempGridVal, GridVY=tempGridVal;
				if(GridVX>=128)
					GridVY=96;
				if(GridVal!= OldGridVal)
				{
					Draw16cGrid(0,tempGridVal,GridVY,PA_RGB(31,31,31),PA_RGB(0,0,31));
					OldGridVal = GridVal;
				}
				SET_FLAG(Settings.GridFlags,GRID_ON);
				RESET_FLAG(Settings.GridFlags,GRID_ZOOMED);
			}
			else 
			{
				if(GridVal<3)
				{
					PA_16cErase(0);
					RESET_FLAG(Settings.GridFlags,GRID_ON);
				}
			}
		}
		//Zoom Slider
		if(tool==TB_Zoom) 
		{
			Draw.Zoom= Box.Tool[tool].SliderValue;
			s16 tempZoom=CheckZoomValue(Draw.Zoom,3);
			if(Settings.AutoGrid && tempZoom!=0)
			{
				Draw.Zoom=tempZoom;
				Draw16cGrid(0,256/tempZoom,256/tempZoom,PA_RGB(31,31,31),PA_RGB(0,0,31));
				SET_FLAG(Settings.GridFlags,GRID_ON);
				SET_FLAG(Settings.GridFlags,GRID_ZOOMED);
			}
			else
			{
				PA_16cErase(0);
				RESET_FLAG(Settings.GridFlags,GRID_ON);
				Draw.Zoom= Box.Tool[TB_Zoom].SliderValue;
			}
			if(DSBmp.Width>=SCREEN_WIDTH)
			{
				DSBmp.ScrollX=DSBmp.CenterX-(Draw.Zoom>>1);
				while(DSBmp.ScrollX>DSBmp.Width+((40*Draw.Zoom)>>8))
					DSBmp.ScrollX--;
				while(DSBmp.ScrollX<-((40*Draw.Zoom)>>8))
					DSBmp.ScrollX++;
				DSBmp.HWScrollX=0;
				UpdateDrawing=1;
			}
			else
			{
				s32 CalcCanvasLowX=DSBmp.CanvasX-((DSBmp.CanvasX*Draw.Zoom)>>8);
				s32 CalcCanvasMaxX= DSBmp.Width+DSBmp.CanvasX-((DSBmp.CanvasX+DSBmp.Width)*Draw.Zoom>>8);
				DSBmp.HWScrollX=DSBmp.CenterX-(Draw.Zoom>>1);	
				if(DSBmp.HWScrollX>CalcCanvasMaxX)
					DSBmp.HWScrollX = CalcCanvasMaxX;
				if(DSBmp.HWScrollX<CalcCanvasLowX)
					DSBmp.HWScrollX = CalcCanvasLowX;
			}
			if(DSBmp.Height>=SCREEN_HEIGHT)
			{
				DSBmp.ScrollY=DSBmp.CenterY-(Draw.Zoom>>1);	
				while(DSBmp.ScrollY>DSBmp.Height+((40*Draw.Zoom)>>8) - ((Draw.Zoom*SCREEN_HEIGHT)>>8))
					DSBmp.ScrollY--;
				while(DSBmp.ScrollY<-((40*Draw.Zoom)>>8))
					DSBmp.ScrollY++;
				DSBmp.HWScrollY=0;
				UpdateDrawing=1;
			}
			else
			{	
				s32 CalcCanvasLowY=DSBmp.CanvasY-((DSBmp.CanvasY*Draw.Zoom)>>8);
				s32 CalcCanvasMaxY= DSBmp.Height+DSBmp.CanvasY-((DSBmp.CanvasY+DSBmp.Height)*Draw.Zoom>>8);
				
				DSBmp.HWScrollY=DSBmp.CenterY-(Draw.Zoom>>1);
				if(DSBmp.HWScrollY>CalcCanvasMaxY)
					DSBmp.HWScrollY = CalcCanvasMaxY;
				if(DSBmp.HWScrollY<CalcCanvasLowY) 
					DSBmp.HWScrollY =CalcCanvasLowY;
			}
			ViewSquare(DSBmp.MiniViewX,DSBmp.MiniViewY,DSBmp.XScale,DSBmp.YScale,0);
			if(UpdateDrawing)
			{
				RefreshDrawing(DSBmp.ScrollX, DSBmp.ScrollY,1);
				UpdateDrawing=0;
				DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
				DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
			}
			Settings.ZoomLevel=Draw.Zoom;
		}
	}
}
//============================================================================================================================================================
//Grid Tool
//============================================================================================================================================================

void DeleteGrid(void)
{
	PA_DeleteBg(0,Layer_Grid);
	SET_FLAG(Settings.GridFlags,GRID_DELETED);
}
void CheckGrid(void)
{
	s32 tempGridVal=0;
	if(GET_FLAG(Settings.GridFlags,GRID_DELETED) && GET_FLAG(Settings.GridFlags,GRID_ON) && GET_FLAG(Settings.GridFlags,GRID_ZOOMED))
	{
		tempGridVal = CheckZoomValue(Draw.Zoom,3);
		if(Draw.Zoom<=32)
			tempGridVal =32;
	}
	if(GET_FLAG(Settings.GridFlags,GRID_DELETED) && GET_FLAG(Settings.GridFlags,GRID_ON) && !GET_FLAG(Settings.GridFlags,GRID_ZOOMED))
		tempGridVal = CheckGridValue(Box.Tool[TB_Grid].SliderValue);
	if(tempGridVal!=0)
	{
		s16 GridVX=tempGridVal, GridVY=tempGridVal;
		
		if(GET_FLAG(Settings.GridFlags,GRID_ZOOMED))
		{
			if(GridVX>=128)
				GridVY=96;
			Draw16cGrid(0,256/tempGridVal,256/GridVY,PA_RGB(31,31,31),PA_RGB(0,0,31));
		}
		else 
			Draw16cGrid(0,tempGridVal,GridVY,PA_RGB(31,31,31),PA_RGB(0,0,31));
		RESET_FLAG(Settings.GridFlags,GRID_DELETED);
	}	
}
void Draw16cGrid(u8 screen,u8 GridSizeX, u8 GridSizeY, u16 color1, u16 color2)
{
	if(GridSizeX>0 && GridSizeY>0)
	{
		ScreenBuf16c=NULL;
		ScreenBuf16c= (u32*) malloc ((26*8*34*4)<<2);
		
		BAG_16cClearZone (0, 0, 0, 256, 192,ScreenBuf16c);	
		int i=0,z=0;
		s8 PixelColor=0;
		s16 TempCols[2]=
		{
			1,
			2,
		};
		//draw horizontal lines
		for(i=GridSizeY;i<SCREEN_HEIGHT-1;i+=GridSizeY)
		{
			PixelColor=0;
			if(i<SCREEN_HEIGHT)
			{
				for(z=0;z<SCREEN_WIDTH;++z)
				{
					BAG_16cPutPixel(screen, z, i,TempCols[PixelColor], ScreenBuf16c);
					PixelColor++;
					if(PixelColor>1)
						PixelColor=0;
				}
			}
		}
		//vertical lines
		for(i=GridSizeX-1;i<SCREEN_WIDTH-1;i+=GridSizeX)
		{
			PixelColor=0;
			if(i<SCREEN_WIDTH)
			{
				for(z=0;z<SCREEN_HEIGHT;++z)
				{
					BAG_16cPutPixel(screen, i, z,TempCols[PixelColor], ScreenBuf16c);
					PixelColor++;
					if(PixelColor>1)
						PixelColor=0;
				}
			}
		}
		DC_FlushRange(ScreenBuf16c, 26*8*34*4);
		
		PA_Init16cBg  (screen, Layer_Grid);
		PA_SetBgPalCol(screen,(0<<4)+1,color1); 
		PA_SetBgPalCol(screen,(0<<4)+2,color2); 
		
		dmaCopy  (ScreenBuf16c,PA_Draw1632[0],26*8*34*4);
		free(ScreenBuf16c);
		ScreenBuf16c=NULL;
	}
}
//============================================================================================================================================================
//Background Updating
//============================================================================================================================================================
void BgLayerUpdate(s16 newcX,s16 newcY)
{
	s16 TempWidth=SCREEN_WIDTH, TempX=0,Height=SCREEN_HEIGHT;
	s16 OffsetX=0;
	if(DSBmp.Height<SCREEN_HEIGHT)
		Height=DSBmp.Height;
	if(DSBmp.Width<TempWidth)
	{
		if(newcX<0)
			TempWidth=DSBmp.Width+newcX;
		else 
			TempWidth=DSBmp.Width;
	}
	if(DSBmp.ScrollX+SCREEN_WIDTH > DSBmp.Width)
		TempWidth=SCREEN_WIDTH-(DSBmp.ScrollX+SCREEN_WIDTH-DSBmp.Width);
	
	else if(DSBmp.ScrollX<0)
	{
		OffsetX=-DSBmp.ScrollX;
		TempWidth=SCREEN_WIDTH+DSBmp.ScrollX;
	}
	int y=0,x=0,ScrnIndex = 0, CanvasIndex = 0;
	//scan colors
	for(y=0;y<Height;++y)
	{
		if(DSBmp.ScrollY+y>=0 && DSBmp.ScrollY+y<DSBmp.Height-1)
		{
			switch(DSBmp.Bits)
			{
				default:
					ScrnIndex  = SCREEN_INDEX(newcX+OffsetX,y+newcY);
					CanvasIndex = CANVAS_INDEX(abs(DSBmp.ScrollX+OffsetX) -TempX,DSBmp.ScrollY+y, DSBmp.Width);	
					memcpy ((u16*)bg_array[0]+ScrnIndex,&bglayer[LayerOrder[0]][CanvasIndex],TempWidth<<1);
				break;
				case 8:
					for(x=0;x<TempWidth;++x){
						ScrnIndex  = SCREEN_INDEX(newcX+OffsetX+x,y+newcY);
						CanvasIndex = CANVAS_INDEX(abs(DSBmp.ScrollX+OffsetX) -TempX + x,DSBmp.ScrollY+y, DSBmp.Width);	
						bg_array[0][ScrnIndex]=palette[bglayer[LayerOrder[0]][CanvasIndex]];
					}
				break;
			}
		}
	}
}

void RefreshDrawing(s32 scrollx, s32 scrolly,bool Fast)
{
	//update the base layer
	memset ((u16*)&bg_array[0],0,SCREEN_WIDTH*SCREEN_HEIGHT*2);
	BgLayerUpdate(DSBmp.CanvasX,DSBmp.CanvasY);
	
	//calculate screen portions to copy
	DSBmp.ScrollX = scrollx; DSBmp.ScrollY = scrolly; //starting coords in bmp
	s16 Width=SCREEN_WIDTH, Height=SCREEN_HEIGHT,OffsetX=0;	
	s16 i=0,y=0, x=0;
	
	if(DSBmp.ScrollX+SCREEN_WIDTH > DSBmp.Width)
		Width=SCREEN_WIDTH-(DSBmp.ScrollX+SCREEN_WIDTH-DSBmp.Width);
	else if(DSBmp.ScrollX<0)
		OffsetX=abs(DSBmp.ScrollX)+DSBmp.CanvasX;
	
	if(DSBmp.Height<SCREEN_HEIGHT)
		Height=DSBmp.Height;
	if(DSBmp.Width<SCREEN_WIDTH)
		Width=DSBmp.Width;
	//copy the screen
	if(!Fast)
	{
		for(i=0;i<DSBmp.NumOfLayers;++i)
		{
			for(y = 0;y<Height;++y)
			{
				if((y+DSBmp.ScrollY)>0 &&(y+DSBmp.ScrollY)<DSBmp.Height-1)
				{
					for(x = 0;x<Width;++x)
					{
						int  Source = bglayer[LayerOrder[i+1]][CANVAS_INDEX(abs(x+DSBmp.ScrollX),y + DSBmp.ScrollY, DSBmp.Width)];
						int ScrnIndex = SCREEN_INDEX(x+DSBmp.CanvasX, y+DSBmp.CanvasY);
						switch(DSBmp.Bits)
						{
							default:
								if(Source>=TransLim)
									bg_array[0][ScrnIndex] = Source;
							break;
							case 8:
								if(Source<Trans8bit)
									bg_array[0][ScrnIndex]=palette[Source];
							break;
						}
					}
				}
			}
		}
	}
	memset ((bool*)&transLines,0,SCREEN_WIDTH*SCREEN_HEIGHT);
}

inline void UpdateBothScreens(void)
{
	RefreshDrawing(DSBmp.ScrollX, DSBmp.ScrollY,0);
	DC_FlushRange(bg_array[0], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
	DMA_Copy(bg_array[0], PA_DrawBg[0], 24576, DMA_32NOW);
	ReloadMiniView(Settings.MiniviewX,Settings.MiniviewY,Settings.MiniviewWidth,Settings.MiniviewHeight);	
	ViewSquare(DSBmp.MiniViewX,DSBmp.MiniViewY,DSBmp.XScale,DSBmp.YScale,0);	
}
inline void ClearScreen(bool screen)
{
	switch(screen)
	{
		case 0:
			DC_FlushRange(PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			dmaFillHalfWords(0,PA_DrawBg[0],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
			PA_ResetSpriteSysScreen(0);
			PA_DeleteBg(0,Layer_Menu);
			DeleteGrid();
			PA_DisableSpecialFx(0);
			PA_SetBgPrio(0,3,3);
			PA_SetBgPrio(0,2,2);
		break;
		case 1:
			DC_FlushRange( ScreenBuf16c,26*8*34*4);
			dmaFillWords(0, ScreenBuf16c, 26*8*34*4);
			DC_FlushRange(PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);			
			dmaFillHalfWords(0,PA_DrawBg[1],(SCREEN_WIDTH*SCREEN_HEIGHT)<<1);	
			PA_ResetSpriteSysScreen(1);
		break;
	}
}

