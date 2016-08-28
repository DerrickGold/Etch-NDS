#ifndef __FONTBAG_H__
#define __FONTBAG_H__

//freetype 2 required stuff
#include <../source/freetype-2.3.12/include/ft2build.h>
#include FT_FREETYPE_H

#define FT_FLOOR(x)     (((x) & -64) / 64)
#define FT_CEIL(x)      ((((x) + 63) & -64) / 64)
#define FT_FIXED(x)     ((x) * 64)
#define _Font_Max_Number 5
#define Font_RGB(r,g,b)	r,g,b



FT_Library ft_lib[5];
FT_Face ft_face[5];

typedef struct{
	u32 size[5];
	u8* buffer[5];
	uint16 *ScreenBuf[2];
	bool Init[5];
}fontinfo;
fontinfo Font;

extern void Font_Init(s8 slot);
extern void Font_FatUnload(s8 slot);
extern void Font_Unload(s8 slot);
extern void Font_Load(u8* font,u32 size_font,s8 slot);
extern bool Font_FatLoad(char *filename,s8 slot);
extern void Font_SetScreenBuf(s8 screen,uint16 *buffer);
extern s8 Font_OutputText(s8 screen,s16 x1,s16 y1,s16 x2, s16 y2,const char *string,s8 AA,int fontsize,s8 slot,u16 color);
#endif
