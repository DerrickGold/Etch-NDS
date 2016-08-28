#include <PA9.h>
#include "bgdata.h"

inline void BAG_16cPutPixel(u8 screen, s16 x, s16 y, u32 color, u32* buffer){
	if(x<SCREEN_WIDTH &&x>=0 && y<SCREEN_HEIGHT && y>=0){
		x += 8; y += 8;
		u16 temp = (x&7)<<2;
		u16 pos = PA_16cPos(x, y);
		buffer[pos] = (color<<temp) | ( (~(15<<temp) ) & buffer[pos]) ;
	}
}
inline void FastLine16c(u8 screen, s16 x, s16 y, s16 x2, s16 y2) {
   	bool yLonger=false;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;				
		yLonger=true;
	}
	int decInc,j=0;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=y;
			for (j=0x8000+(x<<16);y<=longLen;++y) {
				BAG_16cPutPixel(screen,j >> 16,y,1,ScreenBuf16c);
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (j=0x8000+(x<<16);y>=longLen;--y) {
			BAG_16cPutPixel(screen,j >> 16,y,1,ScreenBuf16c);			
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) {
		longLen+=x;
		for (j=0x8000+(y<<16);x<=longLen;++x) {
			BAG_16cPutPixel(screen,x,j >> 16,1,ScreenBuf16c);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (j=0x8000+(y<<16);x>=longLen;--x) {
		BAG_16cPutPixel(screen,x,j >> 16,1,ScreenBuf16c);
		j-=decInc;
	}
}
void BAG_16cClearZone(u8 screen, s16 x1, s16 y1, s16 x2, s16 y2, u32 *buffer){
	x1 += 8;
	y1 += 8;
	y2 += 8;
	u16 temp, pos, temp2, pos2;
	int i, j;
	s16 minx = x1 & (0xffffffff - 7);
	if (minx != x1) minx += 8;
	s16 maxx = x2 & (0xffffffff - 7);

	pos = PA_16cPos(x1, y1);
	temp = (x1&7) << 2;
	pos2 = PA_16cPos(x2, y1);
	temp2 = (x2&7) << 2;	
	for (j = 0; j <= y2-y1; j ++){
		buffer[pos] &= (0xffffffff>>(32-temp));
		buffer[pos+(26*8)] &= (0xffffffff<<temp);
		pos ++;
		buffer[pos2] &= (0xffffffff>>(32-temp2));
		buffer[pos2+(26*8)] &= (0xffffffff<<temp2);
		pos2 ++;
	}

	for (i = minx; i <= maxx; i += 8){ // 8 by 8...
		pos = PA_16cPos(i, y1);	
		for (j = 0; j <= y2-y1; j ++, pos ++)
			buffer[pos] = 0;
	}
}
void ViewSquare(s16 x1,s16 y1,s16 xfactor, s16 yfactor, u16 color){	
	s16 width=((SCREEN_WIDTH/xfactor)*Draw.Zoom)>>8;
	s16 height=(((SCREEN_HEIGHT/yfactor)*Draw.Zoom)>>8);
	
	s16 ScrollX = ((DSBmp.ScrollX+DSBmp.HWScrollX)/xfactor)+x1;
	s16 ScrollY = ((DSBmp.ScrollY+DSBmp.HWScrollY)/yfactor)+y1;
	
	s16 ScrollX2= ScrollX+width;
	s16 ScrollY2=ScrollY+height;
	
	if(DSBmp.Width<SCREEN_WIDTH){	
		ScrollX = ((DSBmp.ScrollX+DSBmp.HWScrollX)/xfactor)+x1-(DSBmp.CanvasX/xfactor);
		ScrollX2=ScrollX+width;
	}
	if(DSBmp.Height<SCREEN_HEIGHT) {
		ScrollY = ((DSBmp.ScrollY+DSBmp.HWScrollY)/yfactor)+y1-(DSBmp.CanvasY/yfactor);
		ScrollY2=ScrollY+height;
	}

	ScreenBuf16c=NULL;
	ScreenBuf16c= (u32*) malloc ((26*8*34*4)<<2);
	
	BAG_16cClearZone (1, 0, 0, 256, 192,ScreenBuf16c);	
	int x=0,y=0,W2 = DSBmp.Width/xfactor, H2 = DSBmp.Height/yfactor;
	for(x=x1;x<=x1+W2;++x){
		for(y=y1;y<=y1+H2;++y)BAG_16cPutPixel(1, x, y, 10,ScreenBuf16c);
	}
	BAG_16cClearZone (1, ScrollX, ScrollY, ScrollX2, ScrollY2,ScreenBuf16c);	

	//top and bottom
	FastLine16c(1,ScrollX,ScrollY,ScrollX2,ScrollY); 
	FastLine16c(1,ScrollX,ScrollY2,ScrollX2,ScrollY2); 

	//left and right
	FastLine16c(1,ScrollX,ScrollY,ScrollX,ScrollY2); 
	FastLine16c(1,ScrollX2,ScrollY,ScrollX2,ScrollY2); 
	
	DC_FlushRange(ScreenBuf16c, 26*8*34*4);
	dmaCopy  (ScreenBuf16c,PA_Draw1632[1],26*8*34*4);
	free(ScreenBuf16c);
	ScreenBuf16c=NULL;
}

void CalculateScale(int imgWidth,int imgHeight,s16 windowWd,s16 windowHt){
	const float ScalePrecision = 1.0;
	
	float xx=1.0, yy=1.0,add =0.0, w=imgWidth,h=imgHeight;
   //if no scaling is needed, center the image 
	if(imgWidth<SCREEN_WIDTH){
		DSBmp.MiniViewX=128-(imgWidth>>1);
	}
	if(imgHeight<SCREEN_HEIGHT){
		DSBmp.MiniViewY=96-(SCREEN_HEIGHT-windowHt)-(imgHeight>>1);
	}
	//scale if needed with no aspect ratio kept
	if(Settings.keepAspect==0){
		if(imgWidth>=SCREEN_WIDTH){
			while(w/xx>windowWd){
				xx++;
			}		
		}
		if(imgHeight>=SCREEN_HEIGHT){
			while(h/yy>windowHt){
				yy++;
			}	
		}
		//screen position
		DSBmp.MiniViewY=96-(SCREEN_HEIGHT-windowHt) -(((int)h/(int)yy)>>1);	
		DSBmp.MiniViewX=128 -(((int)w/(int)xx)>>1);
	}
	//keep aspect ratio scaling
	if(Settings.keepAspect==1){	
		//scaling settings
		if(imgWidth>=SCREEN_WIDTH || imgHeight>=SCREEN_HEIGHT){
			s8 Scale=0;
			if(w/h<=1.1)Scale=1;
			else if(w/h>1.1)Scale=2;
			
			if(Scale==1){//portrait mode
				//calculate y scale
				float NewY=(h/windowHt);
				while(NewY*1.0>1.0){
					NewY-=1.0;
				}
				add = 1.0-NewY;
				yy=(h/windowHt)+add;
				if(windowHt>Settings.MiniviewHeight && add>0.5)yy+=1.0;
				while(w/xx> ((w*windowHt)/h)){
					xx+=ScalePrecision;
				}						
			}
			if(Scale==2){
				//calculate x scale
				float NewX=(w/windowWd);
				while(NewX*1.0>1.0){
					NewX-=1.0;
				}
				add = 1.0-NewX;
				xx=(w/windowWd)+add;
				if(windowWd>Settings.MiniviewWidth && add>0.5)xx+=1.0;
				while(h/yy> ((h*windowWd)/w)){
					yy+=ScalePrecision;
				}
			}
			while(w/xx> windowWd || h/yy> windowHt){
				yy+=ScalePrecision;
				xx+=ScalePrecision;
			}
			//screen position
			DSBmp.MiniViewY=96-(SCREEN_HEIGHT-windowHt) -(((int)h/(int)yy)>>1);	
			if(DSBmp.MiniViewY<0)DSBmp.MiniViewY=0;
			DSBmp.MiniViewX=128 -(((int)w/(int)xx)>>1);
			if(DSBmp.MiniViewX<0)DSBmp.MiniViewX=0;
		}
	}
	DSBmp.XScale=(int)xx;
	DSBmp.YScale=(int)yy;
}
void TimedReloadMiniView(s16 time){
	bool cont=0;
	if(time>0){
		if(!(Stylus.Held)){
			if(DSBmp.MiniViewTimer< time)DSBmp.MiniViewTimer++;//start counter
			else if(DSBmp.MiniViewTimer>=time)cont=1;//update image
		}
		else {
			DSBmp.MiniViewTimer=0;
			DSBmp.UpdateView=0;
		}
	}
	if(cont)MiniViewSection(DSBmp.MiniViewX,DSBmp.MiniViewY);
}
void ReloadMiniView(s16 x1,s16 y1, s16 windowWd, s16 windowHt){
   s32 x=0, y=0;
   if(Settings.ShowMiniView==1){
		y1=DSBmp.MiniViewY;
		x1=DSBmp.MiniViewX;
		memset ((u16*)&bg_array[1],0,SCREEN_WIDTH*SCREEN_HEIGHT*2);
		int i=0;
		
		u16 Transparent=Trans8bit;
		if(DSBmp.Bits>8)Transparent=Trans24bit;
		
		for(y = 0; y < DSBmp.Height; y+=DSBmp.YScale){
			for(x = 0; x < DSBmp.Width; x +=DSBmp.XScale){
				u16 NewX= (x/DSBmp.XScale)+x1;
				u16 NewY= (y/DSBmp.YScale)+y1;
				if(NewX>=0 && NewX<256 && NewY>=0 && NewY<176){
					for(i=0;i<=DSBmp.NumOfLayers;i++){
						switch(DSBmp.Bits){
							default:	
								if(bglayer[LayerOrder[i]][((x)) + (((y)) * DSBmp.Width)]>=TransLim)
									bg_array[1][NewX + (NewY*SCREEN_WIDTH)] =bglayer[LayerOrder[i]][x + (y * DSBmp.Width)];
							break;
							case 8:
								if(bglayer[LayerOrder[i]][((x)) + (((y)) * DSBmp.Width)]<256)
									bg_array[1][NewX+(NewY*SCREEN_WIDTH)]=palette[bglayer[LayerOrder[i]][x + (y* DSBmp.Width)]];
							break;
						}
					}
				}
			}
		}		
		DC_FlushRange(bg_array[1], (SCREEN_WIDTH*SCREEN_HEIGHT)<<1);
		DMA_Copy(bg_array[1], PA_DrawBg[1], 24576, DMA_32NOW);

		DSBmp.MiniViewTimer=0;
		DSBmp.UpdateView=0;
	}
}   
void MiniViewSection(s16 x1,s16 y1){
	s32 x=0,xx=((SCREEN_WIDTH*Draw.Zoom)>>8);
	s32 y=0, yy = ((SCREEN_HEIGHT*Draw.Zoom)>>8);
	u16 Color=0;
	u16 NewX=0, NewY=0;
	if(Settings.ShowMiniView==1){
		for(y = DSBmp.HWScrollY; y < DSBmp.HWScrollY+yy; y+=DSBmp.YScale){
			for(x = DSBmp.HWScrollX; x < DSBmp.HWScrollX+xx; x +=DSBmp.XScale){
				if(x<256 && x>=0 && y>=0 && y<176){
					Color=bg_array[0][(x) + ((y) * SCREEN_WIDTH)];	
					NewX = (((x+DSBmp.ScrollX-DSBmp.CanvasX)/DSBmp.XScale)+x1);
					NewY= (((y+DSBmp.ScrollY-DSBmp.CanvasY)/DSBmp.YScale)+y1);
					bg_array[1][NewX + (NewY*SCREEN_WIDTH)] = Color;
				}
			}
			NewX = (((DSBmp.HWScrollX+DSBmp.ScrollX-DSBmp.CanvasX)/DSBmp.XScale)+x1);
			DC_FlushRange((u16*)&bg_array[1][NewX + (NewY<<8)], (xx/DSBmp.XScale)<<1);
			dmaCopy  ((u16*)&bg_array[1][NewX + (NewY<<8)],(u16*)&PA_DrawBg[1][NewX + (NewY<<8)],(xx/DSBmp.XScale<<1));		
		}		

		DSBmp.MiniViewTimer=0;
		DSBmp.UpdateView=0;
	}
}  
 