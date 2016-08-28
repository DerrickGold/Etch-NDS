#include <PA9.h>
#include "bgdata.h"


void Font_FatUnload(s8 slot){
	//reset freetype memory stuff
	//FT_Bitmap_Done(ft_lib[slot],ft_face[slot]->glyph->bitmap);
	
	FT_Done_Face(ft_face[slot]);
	ft_face[slot]=NULL;
	FT_Done_FreeType(ft_lib[slot]);
	ft_lib[slot]=NULL;
	
	//clear font buffer
	free(Font.buffer[slot]);
	Font.buffer[slot]=NULL;
	Font.size[slot]=0;
	Font.Init[slot]=0;
}
	
void Font_FatInit(s8 slot){
	if(Font.Init[slot]==1)Font_FatUnload(slot);
	Font.buffer[slot]=NULL;
	if(ft_lib[slot]==NULL)FT_Init_FreeType(&ft_lib[slot]);
	Font.Init[slot]=1;
}

bool _FontFatLoad(const char *filename,s8 slot){
	FILE* fontFile = fopen (filename, "rb"); //rb = read
	if(fontFile){ // Make sure there is a file to load
		Font_FatInit(slot);//initiate the library
		// obtain file size:
		fseek (fontFile, 0 , SEEK_END);
		Font.size[slot] = ftell (fontFile);
		rewind (fontFile);
		// allocate memory to store the image file
		Font.buffer[slot] = (u8*)calloc(Font.size[slot], sizeof(u8));
		if(Font.buffer[slot] ==NULL){
			fclose (fontFile);
			BAG_Error_Print(Error_FontLoadAlloc);
			return 0;
		}
		fread (Font.buffer[slot], 1, Font.size[slot], fontFile);
		// close the file
		fclose (fontFile);
		return 1;
		
	}
	return 0;
}

	
void _FontInit(u32 size_font,u8* mem_base,s8 slot){
    FT_Open_Args openArgs = {
	.flags = FT_OPEN_MEMORY,
	.memory_size = size_font,
	.memory_base = mem_base,
    };
    FT_Open_Face(ft_lib[slot], &openArgs, 0, &ft_face[slot]);
}
void Font_Unload(s8 slot){
	FT_Done_Face(ft_face[slot]);
	ft_face[slot]=NULL;
	FT_Done_FreeType(ft_lib[slot]);
	ft_lib[slot]=NULL;
	Font.Init[slot]=0;
	Font.buffer[slot]=NULL;
}

void Font_Init(s8 slot){
	if(Font.Init[slot])Font_Unload(slot);
	ft_face[slot]=NULL;
	ft_lib[slot]=NULL;
	FT_Init_FreeType(&ft_lib[slot]);
	Font.Init[slot]=1;
}
		
void Font_Load(u8* font,u32 size_font,s8 slot){
	Font_Init(slot);
	_FontInit(size_font,font,slot);
}

bool Font_FatLoad(char *filename,s8 slot){
	if(_FontFatLoad(filename,slot)){
		_FontInit(Font.size[slot],(u8*)Font.buffer[slot],slot);
		return 1;
	}
	return 0;
}
void Font_SetScreenBuf(s8 screen,uint16 *buffer){
	Font.ScreenBuf[screen]=buffer;
}

void video_draw_ftbitmap16bit(s8 screen,s16 x, s16 y, FT_Bitmap *bitmap,bool AA,s16 red,s16 green, s16 blue){
    uint8 *src = bitmap->buffer;
    uint16 *dest = Font.ScreenBuf[screen] + x + SCREEN_WIDTH * y;
    s16 width = bitmap->width / 3;
    s16 height = bitmap->rows;
    s16 dest_gap = SCREEN_WIDTH - width;
    s16 src_gap = bitmap->pitch - bitmap->width;
    s16 x1,y1;
	bool SkipAA=0;
	
	for(y1=height;y1>0;y1--){
		for (x1=0; x1<width; x1++) {
			s16 alpha = *(src++);
			src+=2;
			if (alpha) {
				if(AA==0){
					if(alpha<170)SkipAA=1;
					else alpha=255;
				}
				if(SkipAA==0){
					if(x<SCREEN_WIDTH && y<SCREEN_HEIGHT){
						u16 color=*dest;
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
							
						*dest = PA_RGB((NewRed1+NewRed2),(NewGreen1+NewGreen2),(NewBlue1+NewBlue2));
					}
				}
				else SkipAA=0;
			}	
			dest++;
		}
		src += src_gap;
		dest += dest_gap;
    }
	
}

s8 video_draw_string(s8 screen,s16 x1, s16 y1,s16 x2, s16 y2, FT_Face face, const char *string,bool AA,s16 red, s16 green, s16 blue){
    int ascent = FT_CEIL(FT_MulFix(face->bbox.yMax, face->size->metrics.y_scale));
    y1 += ascent;
	s32 startX=x1;
	s32 x=x1,y=y1;
	s32 TempX=0;
	s16 i=0;
    for(i=0;string[i]!=0;i++) {
		if(string[i]=='\n'){
			y += ascent; 
			x=startX;
		}
		else if(string[i]==' '){
			x += FT_CEIL(face->glyph->metrics.horiAdvance);
			if(x>x2){//if text is going off screen, move down a line
				y += ascent; 
				x=startX;
			}
		}
		else{		
			FT_Load_Char(face, string[i], FT_LOAD_RENDER | FT_LOAD_TARGET_LCD);
				if(x + FT_CEIL(face->glyph->metrics.horiAdvance)>x2){//if text is going off screen, move down a line
					y += ascent;					
					x=startX;
				}
				if(y>y2)return i;
				TempX=x + FT_FLOOR(face->glyph->metrics.horiBearingX);				
				while(TempX<x1){
					x++;
					TempX= x + FT_FLOOR(face->glyph->metrics.horiBearingX);
				}
					video_draw_ftbitmap16bit(screen,TempX,
								y - FT_FLOOR(face->glyph->metrics.horiBearingY),
								&face->glyph->bitmap,AA,red, green, blue);
			x += FT_CEIL(face->glyph->metrics.horiAdvance);
		}
		
    }
	return i;//return how many characters displayed
}

s8 Font_OutputText(s8 screen,s16 x1,s16 y1,s16 x2, s16 y2,const char *string,s8 AA,int fontsize,s8 slot,u16 color){
	s8 red=color&31;
	s8 green=color>>5&31;
	s8 blue=color>>10&31;
	FT_Set_Char_Size(ft_face[slot], 0, fontsize, 0, 0);
	return (video_draw_string(screen,x1, y1,x2,y2,ft_face[slot],string,AA,red,green,blue));
}
