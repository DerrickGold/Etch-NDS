#include <PA9.h>
#include "bgdata.h"

#define ClrSliderLx 81

// Sprites:
extern const unsigned char cw1_Sprite[4096] _GFX_ALIGN; // 16bit Sprite
extern const unsigned char cw2_Sprite[4096] _GFX_ALIGN; // 16bit Sprite
extern const unsigned char cw3_Sprite[4096] _GFX_ALIGN; // 16bit Sprite
extern const unsigned char cw4_Sprite[4096] _GFX_ALIGN; // 16bit Sprite

u8* colorwheelgfx[4]=
{
	(u8*) cw1_Sprite,
	(u8*) cw2_Sprite,
	(u8*) cw3_Sprite,
	(u8*) cw4_Sprite,
};

u8 GetPalNumFromSquare(s16 x,s16 y)
{
	int num = ((x-114)>>3)+(((y-28)>>3)*16);
	if(num<DSBmp.NColors)
		return num;
		
	return DSBmp.NColors;
}

u16 Get16BitSpriteCol(u8 width,u8 x, u8 y)
{
	s8 TempSprite=0;
	s16 CurrentXPos=0,CurrentYPos=0;
	if(x<CurrentXPos+64 && y<CurrentYPos+64)TempSprite=0;
	else if(x>=ColorWheel[0].X+64 && y<ColorWheel[0].Y+64)TempSprite=1;
	else if(x<ColorWheel[0].X+64 && y>=ColorWheel[0].Y+64)TempSprite=2;
	else if(x>=ColorWheel[0].X+64 && y>=ColorWheel[0].Y+64)TempSprite=3;
	CurrentXPos=ColorWheel[TempSprite].X;
	CurrentYPos=ColorWheel[TempSprite].Y;
	switch(TempSprite)
	{
		case 0:
			return sprite1_GFX[x-CurrentXPos+((y-CurrentYPos)*width)];
		break;
		case 1:
			return sprite2_GFX[x-CurrentXPos+((y-CurrentYPos)*width)];
		break;
		case 2:
			return sprite3_GFX[x-CurrentXPos+((y-CurrentYPos)*width)];
		break;	
		case 3:
			return sprite4_GFX[x-CurrentXPos+((y-CurrentYPos)*width)];
		break;
	}
	
	return 0;
}
u16 PA_GetSpritePalCol(u8 screen, u8 pal_number, u8 color_number) 
{
	u16 *palcolor;
	u16 tempcolor=0;
	if (screen == 0) 
	{
		vramSetBankG(VRAM_G_LCD);	// bank contents can only be changed in LCD mode
		palcolor = (u16*)(VRAM_G + (pal_number << 8));
		tempcolor= palcolor[color_number];
		vramSetBankG(VRAM_G_SPRITE_EXT_PALETTE);
		return tempcolor;
	} 
	else
	{
		vramSetBankI(VRAM_I_LCD);	// bank contents can only be changed in LCD mode
		palcolor = (u16*)(VRAM_I + (pal_number << 8));
		tempcolor= palcolor[color_number];
		vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);
		return tempcolor;
	}
	return 0;
}
void CreateColorWheel(s16 x,s16 y)
{
	ColorWheel[0].X=x;ColorWheel[1].X=x+64;
	ColorWheel[2].X=x;ColorWheel[3].X=x+64;
	ColorWheel[0].Y=y;ColorWheel[1].Y=y;
	ColorWheel[2].Y=y+64;ColorWheel[3].Y=y+64;
	
	ColorWheel[0].Sprite=ClrMenuSprStrt;ColorWheel[1].Sprite=ClrMenuSprStrt+1;ColorWheel[2].Sprite=ClrMenuSprStrt+2;ColorWheel[3].Sprite=ClrMenuSprStrt+3;
	
	int i=0;
	for(i=0;i<4;i++)
	{
		PA_Create16bitSprite(0,ColorWheel[i].Sprite,(void*)colorwheelgfx[i],OBJ_SIZE_64X64,ColorWheel[i].X,ColorWheel[i].Y);
		PA_Set16bitSpriteAlpha (0,ColorWheel[i].Sprite,0);
	}
}
void CreateLayerBar(s8 screen, s16 x, s16 y, s8 pal, u8 startsprite)
{
	s8 i=0;
	PA_LoadSpritePal(screen,pal,(void*)layerbutton_Pal);	
	for(i=0;i<2;i++)
	{
		LayerBar[i].Screen = screen;
		LayerBar[i].Pal = pal;
		LayerBar[i].Sprite= startsprite+i;
		LayerBar[i].X = x +(i*64);
		LayerBar[i].Y = y;
		PA_CreateSprite(screen,LayerBar[i].Sprite,(void*)layerbutton_Sprite,OBJ_SIZE_64X32,1,pal,LayerBar[i].X,LayerBar[i].Y);
		PA_SetSpriteAnimEx (screen, LayerBar[i].Sprite,64,32,1,i);
		PA_SetSpriteXY(screen, LayerBar[i].Sprite, LayerBar[i].X ,LayerBar[i].Y);
		HideSprite(0,LayerBar[i].Sprite,0);
	}
	PA_InitAllSpriteDraw(); 
	char textbuf[256];
	sprintf(textbuf,"%d/%d",DSBmp.LayerSelected,DSBmp.NumOfLayers);
	PA_SpriteText(screen,LayerBar[0].Sprite,31,16+5,64,32,textbuf,1,5,1,6);
}
void UpdateLayerBar(void)
{
	if(Stylus.Newpress && PA_SpriteTouched(LayerBar[1].Sprite))
	{
		if(PA_StylusInZone(LayerBar[1].X+6,LayerBar[1].Y+17,LayerBar[1].X+30,LayerBar[1].Y+31))
		{
			DSBmp.LayerSelected++;
			if(DSBmp.LayerSelected> DSBmp.NumOfLayers)DSBmp.LayerSelected--;
			CreateLayerBar(0, 66, 160, LayerButtonData);
		}
		else 	if(PA_StylusInZone(LayerBar[1].X+31,LayerBar[1].Y+17,LayerBar[1].X+54,LayerBar[1].Y+31))
		{
			DSBmp.LayerSelected--;
			if(DSBmp.LayerSelected<0)DSBmp.LayerSelected++;
			CreateLayerBar(0, 66, 160, LayerButtonData);
		}
	}
}

void LoadColorInterFace(void)
{
	DSBmp.PrgmLocation = Location_Color;
	int i=0;
	DeleteGrid();
	
	//load palette views
	if(DSBmp.Bits>8)
	{//Load the 10 color palette
		PA_LoadBackground(0, Layer_Menu, &maininterface);
		CreateColorWheel(48,15);
		//load color squares
		PA_LoadSpritePal(0,1,(void*)colorsquare_Pal);	
		for(i=0;i<10;i++)
		{
			if(i<5)PA_CreateSprite(0,ClrMenuSprStrt+i+9,(void*)colorsquare_Sprite,OBJ_SIZE_16X16,1,1,156,23+(i*19));
			else PA_CreateSprite(0,ClrMenuSprStrt+i+9,(void*)colorsquare_Sprite,OBJ_SIZE_16X16,1,1,175,23+((i-5)*19));
			PA_SetSpriteAnimEx (0, ClrMenuSprStrt+i+9,16,16,1,i);
			if(Palettes[i]!=0)PA_SetSpritePalCol(0, 1,i+1,Palettes[i]);	
		}
		PalSliders[0].Y=127;
		PalSliders[0].X=60;
		PalSliders[1].X=202;
	}
	else //set the 256 color palette
	{
		PA_SetBgRot(0, Layer_Draw,0,0,0,0,0, 256);
		PA_LoadBackground(0, Layer_Menu, &_256colormenu);
		CreateColorWheel(15,14);
		//draw background square
		PalSliders[0].Y=124;
		PalSliders[0].X=25;
		//place color tiles
		int y=0,x=0,tiles=0;
		for(y=0;y<16;y++)
		{
			for(x=0;x<16;x++)
			{
				Fill16bitBlock(114+(x*8),28+(y*8),8,8,palette[tiles],PA_DrawBg[0]);
				PalPos[tiles].x=114+(x*8); PalPos[tiles].y=28+(y*8);
				tiles++;
			}
		}
		PA_16bitText(0,18,128, SCREEN_WIDTH,192, "R", PA_RGB(31,0,0), 5, 1, 100);	
		PA_16bitText(0,18,144, SCREEN_WIDTH,192, "G", PA_RGB(0,31,0), 5, 1, 100);
		PA_16bitText(0,18,160, SCREEN_WIDTH,192, "B", PA_RGB(0,0,31), 5, 1, 100);
	}
		
	//load sliders
	PA_LoadSpritePal(0,0,(void*)sliders_Pal);
	for(i=0;i<3;i++)
		PA_CreateSprite(0,ClrMenuSprStrt+i+4,(void*)sliders_Sprite,OBJ_SIZE_16X16,1,0,PalSliders[0].X,PalSliders[0].Y+(i*16));

	if(DSBmp.Bits>8)//create alpha slider
	{
		PA_CreateSprite(0,ClrMenuSprStrt+7,(void*)sliders_Sprite,OBJ_SIZE_16X16,1,0,PalSliders[1].X,33);
		PA_SetSpriteAnimEx (0,ClrMenuSprStrt+7,16,16,1,1);
		PA_SetSpriteY(0,ClrMenuSprStrt+7,129-((Draw.Alpha * 129)/256)+32);
	}
	//set slider positions
	u16 Color= Draw.Color[Box.SwapPal];
	if(DSBmp.Bits<=8)
		Color = palette[Draw.Color[Box.SwapPal]];
		
	//populate selected color positions
	s8 red=Color&31;
	PA_SetSpriteX(0,ClrMenuSprStrt+4,((red*ClrSliderLx)/32)+PalSliders[0].X);
	s8 green=Color>>5&31;
	PA_SetSpriteX(0,ClrMenuSprStrt+5,((green*ClrSliderLx)/32)+PalSliders[0].X);
	s8 blue=Color>>10&31;
	PA_SetSpriteX(0,ClrMenuSprStrt+6,((blue*ClrSliderLx)/32)+PalSliders[0].X);
	
	PA_CreateSprite(0,ClrMenuSprStrt+8,(void*)sliders_Sprite,OBJ_SIZE_16X16,1,0,156,23);
	PA_SetSpriteAnimEx (0,ClrMenuSprStrt+8,16,16,1,2);
	HideSprite(0,ClrMenuSprStrt+8,1);
	
	CreateLayerBar(0, 66, 160, LayerButtonData);
}


void RunColorInterface(u8* ButtonHeld)
{
	//FastDelTB();
	PA_EnableSpecialFx(0,SFX_ALPHA,SFX_OBJ | SFX_BG2 | SFX_BG1 ,SFX_BG0 | SFX_BG3 | SFX_BD); // Everything normal
	PA_SetSFXAlpha(0,0,16);	
	LoadColorInterFace();
	
	FadeInInterface(1);
	u16 TempColor=0,TempColor2=0;
	u8 PaletteSelected=Draw.Color[Box.SwapPal];
	
	s8 EditSquarePalCol = 4;
	
	if(DSBmp.Bits>8)
		TempColor=Draw.Color[Box.SwapPal];
	else 
	{//edit square color
		TempColor=palette[Draw.Color[Box.SwapPal]];
		HideSprite(0,ClrMenuSprStrt+8,0);
		PA_SetSpriteAnimEx (0,ClrMenuSprStrt+8,16,16,1,3);
		PA_SetSpriteXY(0,ClrMenuSprStrt+8,PalPos[PaletteSelected].x,PalPos[PaletteSelected].y);
		s8 red=TempColor&31;
		s8 green=TempColor>>5&31;
		s8 blue=TempColor>>10&31;
		PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));
	}
	s8 red=TempColor&31;
	s8 green=TempColor>>5&31;
	s8 blue=TempColor>>10&31;
	
	s8 sprite=0,Edit=1,TempSprite=0;
	s16 Brightness=0;
	bool adjustBrightness=0;
	
	while(*ButtonHeld)
	{
		if(Stylus.Newpress)
		{
			if(DSBmp.Bits>8)
			{
				if(PA_StylusInZone(156,116,190,130))
				{
					Edit++;
					if(Edit>1)
						Edit=0;
					HideSprite(0,ClrMenuSprStrt+8,Edit);
					TempColor=PA_GetSpritePalCol(0,1,TempSprite+1);
					Draw.Color[Box.SwapPal]=TempColor;		
					UpdateColorSquares(Box.SwapPal);
					//update slider positions
					red=TempColor&31;
					green=TempColor>>5&31;
					blue=TempColor>>10&31;
					PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));//edit square color
					PA_SetSpriteX(0,ClrMenuSprStrt+4,((red*ClrSliderLx)/32)+PalSliders[0].X);
					PA_SetSpriteX(0,ClrMenuSprStrt+5,((green*ClrSliderLx)/32)+PalSliders[0].X);
					PA_SetSpriteX(0,ClrMenuSprStrt+6,((blue*ClrSliderLx)/32)+PalSliders[0].X);										
				}

				for(sprite=0;sprite<10;sprite++)
				{
					if(PA_SpriteTouched(ClrMenuSprStrt+sprite+9))
					{
						if(sprite<5)
							PA_SetSpriteXY(0,ClrMenuSprStrt+8,156,23+(sprite*19));
						else 
							PA_SetSpriteXY(0,ClrMenuSprStrt+8,175,23+((sprite-5)*19));
							
						TempSprite=sprite; 
						TempColor=PA_GetSpritePalCol(0,1,sprite+1);
						Draw.Color[Box.SwapPal]=TempColor;		
						UpdateColorSquares(Box.SwapPal);
						//update slider positions
						red=TempColor&31;
						green=TempColor>>5&31;
						blue=TempColor>>10&31;
						PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));//edit square color
						PA_SetSpriteX(0,ClrMenuSprStrt+4,((red*ClrSliderLx)/32)+PalSliders[0].X);
						PA_SetSpriteX(0,ClrMenuSprStrt+5,((green*ClrSliderLx)/32)+PalSliders[0].X);
						PA_SetSpriteX(0,ClrMenuSprStrt+6,((blue*ClrSliderLx)/32)+PalSliders[0].X);								
					}
				}
			}
		}
		if(Stylus.Held)
		{
			if(DSBmp.Bits==8)
			{
				if(PA_StylusInZone(116,28,241,155))
				{
					PaletteSelected=GetPalNumFromSquare(Stylus.X,Stylus.Y);
					PA_SetSpriteXY(0,ClrMenuSprStrt+8,PalPos[PaletteSelected].x,PalPos[PaletteSelected].y);
					TempColor=palette[PaletteSelected];
					Draw.Color[Box.SwapPal]=PaletteSelected;
					UpdateColorSquares(Box.SwapPal);
					red=TempColor&31;
					green=TempColor>>5&31;
					blue=TempColor>>10&31;
					PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));//edit square color
					PA_SetSpriteX(0,ClrMenuSprStrt+4,((red*ClrSliderLx)/32)+PalSliders[0].X);
					PA_SetSpriteX(0,ClrMenuSprStrt+5,((green*ClrSliderLx)/32)+PalSliders[0].X);
					PA_SetSpriteX(0,ClrMenuSprStrt+6,((blue*ClrSliderLx)/32)+PalSliders[0].X);	
				}
			}
			if((DSBmp.Bits>8 &&PA_TrueDistance (95,62,Stylus.X,Stylus.Y)<46) || (DSBmp.Bits==8 && PA_TrueDistance (60,61,Stylus.X,Stylus.Y)<46))
			{
				if(Edit==1)
				{
					switch(DSBmp.Bits)
					{
						default:
							Draw.Color[Box.SwapPal]=Get16BitSpriteCol(64,Stylus.X,Stylus.Y);
							TempColor=Draw.Color[Box.SwapPal];
							UpdateColorSquares(Box.SwapPal);
						break;
						case 8:
							palette[PaletteSelected]=Get16BitSpriteCol(64,Stylus.X,Stylus.Y);
							TempColor=palette[PaletteSelected];
							Draw.Color[Box.SwapPal]=PaletteSelected;
							UpdateColorSquares(Box.SwapPal);
							Fill16bitBlock(PalPos[PaletteSelected].x,PalPos[PaletteSelected].y,8,8,palette[PaletteSelected],PA_DrawBg[0]);
							
							if(PaletteSelected == DSBmp.NColors)
								DSBmp.NColors++;
						break;
					}
				}
				else if(Edit==0)
				{
					PA_SetSpritePalCol(0, 1,TempSprite+1,Get16BitSpriteCol(64,Stylus.X,Stylus.Y));
					Palettes[TempSprite] = Get16BitSpriteCol(64,Stylus.X,Stylus.Y);
					TempColor=Palettes[TempSprite];	
					Draw.Color[Box.SwapPal]=TempColor;
					UpdateColorSquares(Box.SwapPal);
				}
				//update slider positions
				red=TempColor&31;
				green=TempColor>>5&31;
				blue=TempColor>>10&31;
				PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));//edit square color
				PA_SetSpriteX(0,ClrMenuSprStrt+4,((red*ClrSliderLx)/32)+PalSliders[0].X);
				PA_SetSpriteX(0,ClrMenuSprStrt+5,((green*ClrSliderLx)/32)+PalSliders[0].X);
				PA_SetSpriteX(0,ClrMenuSprStrt+6,((blue*ClrSliderLx)/32)+PalSliders[0].X);
			}
			//red	
			if(PA_StylusInZone(PalSliders[0].X-2,PalSliders[0].Y-2,PalSliders[0].X+82,PalSliders[0].Y+12))
			{
				PA_SetSpriteX(0,ClrMenuSprStrt+4,Stylus.X);
				red=(Stylus.X-PalSliders[0].X)/2.6;
				if(red>31)
					red=31;
				if(Edit==1)
				{
					switch(DSBmp.Bits)
					{
						default:
							Draw.Color[Box.SwapPal]=PA_RGB(red,green,blue);
							TempColor=Draw.Color[Box.SwapPal];
							UpdateColorSquares(Box.SwapPal);
						break;
						case 8:
							palette[PaletteSelected]=PA_RGB(red,green,blue);
							TempColor=palette[PaletteSelected];
							Draw.Color[Box.SwapPal]=PaletteSelected;
							UpdateColorSquares(Box.SwapPal);
							Fill16bitBlock(PalPos[PaletteSelected].x,PalPos[PaletteSelected].y,8,8,palette[PaletteSelected],PA_DrawBg[0]);
							
							if(PaletteSelected == DSBmp.NColors)
								DSBmp.NColors++;
						break;
					}
				}
				else
				{
					PA_SetSpritePalCol(0, 1,TempSprite+1,PA_RGB(red,green,blue));
					Palettes[TempSprite] = PA_RGB(red,green,blue);
					TempColor=Palettes[TempSprite];	
					PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));//edit square color
					Draw.Color[Box.SwapPal]=TempColor;
					UpdateColorSquares(Box.SwapPal);
				}
			}
			//green
			if(PA_StylusInZone(PalSliders[0].X-2,PalSliders[0].Y+14,PalSliders[0].X+82,PalSliders[0].Y+28))
			{
				PA_SetSpriteX(0,ClrMenuSprStrt+5,Stylus.X);
				green=(Stylus.X-PalSliders[0].X)/2.6;
				if(red>31)
					red=31;
				if(Edit==1)
				{
					switch(DSBmp.Bits)
					{
						default:
							Draw.Color[Box.SwapPal]=PA_RGB(red,green,blue);
							TempColor=Draw.Color[Box.SwapPal];
							UpdateColorSquares(Box.SwapPal);
						break;
						case 8:
							palette[PaletteSelected]=PA_RGB(red,green,blue);
							TempColor=palette[PaletteSelected];
							Draw.Color[Box.SwapPal]=PaletteSelected;
							UpdateColorSquares(Box.SwapPal);
							Fill16bitBlock(PalPos[PaletteSelected].x,PalPos[PaletteSelected].y,8,8,palette[PaletteSelected],PA_DrawBg[0]);
							
							if(PaletteSelected == DSBmp.NColors)
								DSBmp.NColors++;
						break;
					}
				}
				else 
				{
					PA_SetSpritePalCol(0, 1,TempSprite+1,PA_RGB(red,green,blue));
					Palettes[TempSprite] = PA_RGB(red,green,blue);
					PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));//edit square color
					TempColor=Palettes[TempSprite];	
					Draw.Color[Box.SwapPal]=TempColor;
					UpdateColorSquares(Box.SwapPal);
				}
			}
			//blue
			if(PA_StylusInZone(PalSliders[0].X-2,PalSliders[0].Y+30,PalSliders[0].X+82,PalSliders[0].Y+44))
			{
				PA_SetSpriteX(0,ClrMenuSprStrt+6,Stylus.X);
				blue=(Stylus.X-PalSliders[0].X)/2.6;
				if(blue>31)
					blue=31;
				if(Edit==1)
				{
					switch(DSBmp.Bits)
					{
						default:
							Draw.Color[Box.SwapPal]=PA_RGB(red,green,blue);
							TempColor=Draw.Color[Box.SwapPal];
							UpdateColorSquares(Box.SwapPal);
						break;
						case 8:
							palette[PaletteSelected]=PA_RGB(red,green,blue);
							TempColor=palette[PaletteSelected];
							Draw.Color[Box.SwapPal]=PaletteSelected;
							UpdateColorSquares(Box.SwapPal);
							Fill16bitBlock(PalPos[PaletteSelected].x,PalPos[PaletteSelected].y,8,8,palette[PaletteSelected],PA_DrawBg[0]);
							
							if(PaletteSelected == DSBmp.NColors)
								DSBmp.NColors++;
						break;
					}
				}
				else 
				{
					PA_SetSpritePalCol(0, 1,TempSprite+1,PA_RGB(red,green,blue));
					Palettes[TempSprite] = PA_RGB(red,green,blue);
					PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));//edit square color
					TempColor=Palettes[TempSprite];	
					Draw.Color[Box.SwapPal]=TempColor;
					UpdateColorSquares(Box.SwapPal);
				}
			}	
			if(DSBmp.Bits>8)
			{
				//alpha slider
				if(PA_StylusInZone(PalSliders[1].X-4,32,PalSliders[1].X+8,160))
				{
					PA_SetSpriteY(0,ClrMenuSprStrt+7,Stylus.Y);
					Draw.Alpha=256-(((Stylus.Y-32)<<8)/128);
					
					if(Draw.Alpha>=254)
						Draw.Alpha=256;
				}
			}

			//brightness
			if(DSBmp.Bits>8&&PA_StylusInZone(50,110,143,123))
				adjustBrightness=1;
				
			else if(DSBmp.Bits==8&&PA_StylusInZone(15,111,108,118))
				adjustBrightness=1;
				
			else 
				adjustBrightness=0;
			
			if(adjustBrightness)
			{
				switch(DSBmp.Bits)
				{
					default:
						Brightness = 92-((Stylus.X-50));
						Draw.Color[Box.SwapPal] =ColorBrightness(TempColor,Brightness);
						red=Draw.Color[Box.SwapPal]&31;
						green=Draw.Color[Box.SwapPal]>>5&31;
						blue=Draw.Color[Box.SwapPal]>>10&31;
						PA_SetSpriteX(0,ClrMenuSprStrt+4,((red*ClrSliderLx)/32)+60);
						PA_SetSpriteX(0,ClrMenuSprStrt+5,((green*ClrSliderLx)/32)+60);
						PA_SetSpriteX(0,ClrMenuSprStrt+6,((blue*ClrSliderLx)/32)+60);
						PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));//edit square color
						UpdateColorSquares(Box.SwapPal);
					break;
					case 8:
						Brightness = 92-((Stylus.X-15));
						palette[PaletteSelected]=ColorBrightness(TempColor,Brightness);
						TempColor2=palette[PaletteSelected];
						Draw.Color[Box.SwapPal]=PaletteSelected;
						red=TempColor2&31;
						green=TempColor2>>5&31;
						blue=TempColor2>>10&31;
						PA_SetSpriteX(0,ClrMenuSprStrt+4,((red*ClrSliderLx)/32)+25);
						PA_SetSpriteX(0,ClrMenuSprStrt+5,((green*ClrSliderLx)/32)+25);
						PA_SetSpriteX(0,ClrMenuSprStrt+6,((blue*ClrSliderLx)/32)+25);
						PA_SetSpritePalCol(0, 0,EditSquarePalCol,PA_RGB(32-red,32-green,32-blue));//edit square color
						UpdateColorSquares(Box.SwapPal);	
						Fill16bitBlock(PalPos[PaletteSelected].x,PalPos[PaletteSelected].y,8,8,palette[PaletteSelected],PA_DrawBg[0]);
						
						if(PaletteSelected == DSBmp.NColors)
							DSBmp.NColors++;
					break;
				}
			}
		
		}
		else if(Stylus.Released)
		{
			if(DSBmp.Bits==8)
			{
				ReloadMiniView(Settings.MiniviewX,Settings.MiniviewY,Settings.MiniviewWidth,Settings.MiniviewHeight);
				UpdateStatsBar();
			}
		}
		UpdateLayerBar();
		PA_WaitForVBL();
	}
	RESET_FLAG(Display.Flag,COLMEN);
	//FadeInterface(1,1);
	FadeOutInterface(1);
	FastMainInterFace();
	//if(Box.Created==1)Box.Created=0;
}
